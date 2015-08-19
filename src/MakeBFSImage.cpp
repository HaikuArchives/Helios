// MakeBFSImage

#include "MakeBFSImage.h"

#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str)	gMSHLangMgr->_T(str).String()
#define _TPS(str)	gMSHLangMgr->_T(str)
#else
#include <locale/Locale.h>
#endif

#include <app/Roster.h>

#include "Application1.h"
#include "ConfigFields.h"
#include "ErrorBox.h"
#include "FileUtils.h"
#include "MString.h"

status_t
MakeBFSImage(const char *filename)
{
	const int buffersize = 1048576;

	Application1* app = dynamic_cast<Application1*>(be_app);

	{
		//+MSH18/08/04: Feature currently enabled but with unmount issues unresolved!
		ErrorBox *eb=new ErrorBox(E_RED_COLOR,	_T("Warning"), // "TWARNING"
				_T("BFS support is *experimental* - use at own risk!"), // "TBFSSUPPORT"
				_T("Ok")); // "TOK"
		eb->Go();
	}

	{
		// Delete image file if it exists (original filename, note...)
		BEntry entry;
		entry.SetTo(filename, B_READ_WRITE);
		if (entry.InitCheck() == B_OK) {
			entry.Remove();
		}
		entry.Unset();
	}

	// Get size of directories and files
	const off_t imgsize = GetBFSImageSize(app->base_folder.String());

	// Create the image file on the hard disk.
	{
		BFile *file = new BFile();
		file->SetTo(filename, B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);

		const status_t resizeReturnValue = file->SetSize(imgsize);
		if (resizeReturnValue == B_DEVICE_FULL) {
			app->errtype=_T("Error"); // "TERROR"
			app->errstring=_T("No space left on target volume to create the image file!"); // "TNOSPACE"
			file->Unset();
			delete file;
			file = NULL;
			return B_ERROR;
		}

		// Release the file, so we can mount it and write to it.
		file->Sync();
		file->Unset();
		delete file;
		file = NULL;
	}

	sync();

	// Create the BFS filesystem within the image file.
	{
		BFile *file = new BFile();
		file->SetTo(filename, B_READ_WRITE);

		// Locking check - sanity check really ;-)
		// If this fails, then something still has a file ref to it.
		if ( file->Lock() != B_OK ) {
				app->errtype=_T("Error"); // "TERROR"
				app->errstring="Could not lock file!";
				file->Unset();
				delete file;
				file = NULL;
				return B_ERROR;
		}
		file->Unlock();

		{
			printf("SIZE: imagesize is %lld bytes\n", imgsize);
			BString tmpstring = "";
			tmpstring << _T("Writing image file") << B_UTF8_ELLIPSIS; // "TCREATINGIMAGEFILE"
			app->ShowStatus(tmpstring.String(), 0);
			app->SetStatusInfo("--- MB/s");
		}

		BStopWatch *watch = new BStopWatch("MBps",true);

		char *buffer = NULL;
		buffer = (char *)malloc(buffersize);
		for (int i = 0; i < buffersize; i++) {
			buffer[i] = 0x0;
		}

		char info[40];
		size_t olddone = 0;
		size_t sizetowrite = buffersize;
		size_t sizeWritten = 0;
		while (sizetowrite > 0) {
			if (app->fStatusWindow->interrupted){
				free(buffer);
				delete watch;
				app->errtype=_T("Error"); // "TERROR"
				app->errstring=_T("You just interrupted and terminated the process."); // "TINTERRUPTEDBYUSER"
				file->Unset();
				delete file;
				file = NULL;
				app->HideStatus();
				return B_ERROR;
			}
			if ((imgsize - sizeWritten) > buffersize) {
				sizetowrite = buffersize;
			} else {
				sizetowrite = imgsize - sizeWritten;
			}
		 	sizeWritten += file->Write((const void *)buffer, sizetowrite);

			if (sizeWritten > (olddone + (5 * buffersize))) {
				sprintf(info,"%0.1f MB/s",((float)sizeWritten - (float)olddone)/(watch->ElapsedTime()*0.953674316));
				olddone = sizeWritten;
				app->SetStatusInfo(info);
				watch->Reset();
			}

			app->SetPercentage((float)sizeWritten*100 / ((float)imgsize));
		}

		file->Sync();
		file->Unset();
		delete file;
		file = NULL;

		sync();

		delete watch;
		free(buffer);
	}

		{
			BFile *fileRead = new BFile();
			fileRead->SetTo(filename, B_READ_ONLY);
			
			// Read beginning and end of file, to force a sync!!
			char tempBuffer[2];
			fileRead->Seek(0, SEEK_SET);
			fileRead->Read(tempBuffer, 1);
			fileRead->Seek(0, SEEK_END);
			fileRead->Read(tempBuffer, 1);

			fileRead->Unset();
			delete fileRead;
			fileRead = NULL;
		}


		const char **args;
		args = (const char **)malloc(sizeof(char *) * (22));

		// create a Be File System
		// initialize the image file as a BFS image
		int arguments = 0;
#ifdef _BEOS_HAIKU_BUILD_		
		args[arguments++] = strdup(app->CM->GetString(PATH_TO_MKBFS)->String());
		args[arguments++] = strdup("-t");
		args[arguments++] = strdup("bfs");
		args[arguments++] = strdup(filename);
		args[arguments++] = strdup(app->window1->view1->GetVolumeName());
		args[arguments] = NULL;
#else
		args[arguments++] = strdup(app->CM->GetString(PATH_TO_MKBFS)->String());
		args[arguments++] = strdup("2048");
		args[arguments++] = strdup(filename);
		args[arguments++] = strdup(app->window1->view1->GetVolumeName());
		args[arguments] = NULL;
#endif
		extern char **environ;
		const thread_id mkbfsThread = load_image(arguments, args, (const char **)environ);
		set_thread_priority(mkbfsThread, 20);
		if (mkbfsThread < 0) {
			app->errtype = _T("Error"); // "TERROR"
#ifdef _BEOS_HAIKU_BUILD_
			app->errstring = "mkfs\n\n";
#else
			app->errstring = "mkbfs\n\n";
#endif			
			app->errstring << _T("Could not load this command line tool. Make sure it is in the correct place."); // "TCOULDNOTLOAD"
			free(args);
			app->HideStatus();
			return B_ERROR;
		}

		{
			BString tmpstring = "";
			tmpstring << _T("Writing image file") << B_UTF8_ELLIPSIS; // "TCREATINGIMAGEFILE"
			app->SetStatus(tmpstring.String(), mkbfsThread);
		}

		printf("HELIOS: creating Be File System\n");

		status_t mkbfsWaitThreadReturn = B_OK;
		wait_for_thread(mkbfsThread, &mkbfsWaitThreadReturn);

		if (app->fStatusWindow->interrupted) {
			free(args);
			app->errtype=_T("Error"); // TERROR"
			app->errstring=_T("You just interrupted and terminated the process."); // "TINTERRUPTEDBYUSER"
			app->HideStatus();
			return B_ERROR;
		}

		entry_ref appRef;

		if ( B_OK == get_ref_for_path("/bin/sync", &appRef)) {
			if (B_OK == be_roster->Launch(&appRef)){
				printf("Synced externally okay after mkbfs\n");
			}
		}

		sync();

	{
		// Create the mount point for the BFS image file.
		printf("HELIOS: creating mount point\n");
		CreateFolder(app->pathCV->GetDefaultMountPoint());
	}

		// mount the image file!
		printf("HELIOS: mounting %s\n", filename);
		if (MountBFSImage(strdup(filename), strdup(app->pathCV->GetDefaultMountPoint()))<0) {
			UnmountBFSImage(strdup(app->pathCV->GetDefaultMountPoint()));
			app->errtype=_T("Error"); // "TERROR"
			app->errstring=filename;
			app->errstring<<"\n\n"<<_T("Could not mount the file system."); // "TMOUNTERROR"
			app->HideStatus();
			free(args);
			return B_ERROR;
		}


		sync();
		snooze(500000);

		// sometimes, freshly created image files aren't empty...
		// so, delete all files, please.
		printf("HELIOS: erasing files in mounted image file\n");
		
		BDirectory mountDir(app->pathCV->GetDefaultMountPoint());
		DeleteAllIn(mountDir);
		mountDir.Unset();


		sync();
		snooze(500000);

//		if ( B_OK == get_ref_for_path("/bin/sync", &appRef)) {
//			if (B_OK == be_roster->Launch(&appRef)){
//			}
//		}



	{
		// copy files into mounted image file
		printf("HELIOS: copying files to image file.\n");
		BString folderStr = app->pathCV->GetDefaultImageName();
		folderStr					= *RemoveTrailingSlash(folderStr);
		int arguments = 0;
		args[arguments++] = strdup("/bin/cp");
		args[arguments++] = strdup("--recursive");
		folderStr.SetTo(app->base_folder.String()).Append(".");
		args[arguments++] = strdup(folderStr.String());
		folderStr.SetTo(app->pathCV->GetDefaultMountPoint()).Append("/");
		args[arguments++] = strdup(folderStr.String());
		args[arguments]=NULL;

		// Create thread to copy files into mounted BFS image.
		const thread_id copyThread = load_image(arguments, args, (const char **)environ);
		set_thread_priority(copyThread, 12);
		if (copyThread < 0) {
			UnmountBFSImage(app->pathCV->GetDefaultMountPoint());
			app->errtype=_T("Error"); // "TERROR"
			app->errstring="cp\n\n";
			app->errstring << _T("Could not load this command line tool. Make sure it is in the correct place."); // "TCOULDNOTLOAD"
			free(args);
			app->HideStatus();
			return B_ERROR;
		}

		{
			BString tmpstring = "";
			tmpstring << _T("Copying files") << B_UTF8_ELLIPSIS; // "TCOPYINGFILES"
			app->SetStatus(tmpstring.String(), copyThread);
		}

		rename_thread(copyThread, CPTHREADNAME);
		resume_thread(copyThread);

		// Wait for copy thread to begin running before proceeding.
		int	timeout = app->applicationCV->GetTimeoutLength();
		timeout = app->applicationCV->GetTimeoutLength()*100;
		while (find_thread(CPTHREADNAME) != copyThread) {
			snooze(100000);
			timeout--;
			if (timeout==0) {
				free(args);
				UnmountBFSImage(app->pathCV->GetDefaultMountPoint());
				app->HideStatus();
				app->errtype=_T("Time out!"); // "TTIMEOUT"
				app->errstring=_T("The active process had to wait too long for an event."); // "TTIMEOUTERROR"
				return B_ERROR;
			}
			if (app->fStatusWindow->interrupted) {
				free(args);
				UnmountBFSImage(app->pathCV->GetDefaultMountPoint());
				app->HideStatus();
				app->errtype=_T("Error"); // "TERROR"
				app->errstring=_T("You just interrupted and terminated the process."); // "TINTERRUPTEDBYUSER"
				return B_ERROR;
			}
			app->SetPercentage((float)timeout*100/((float)app->applicationCV->GetTimeoutLength()*100));
		}

		// COPYING FILES
		while (find_thread(CPTHREADNAME) == copyThread) {
			sleep(1);
			if (app->fStatusWindow->interrupted) {
				free(args);
				UnmountBFSImage(app->pathCV->GetDefaultMountPoint());
				app->HideStatus();
				app->errtype=_T("Error"); // "TERROR"
				app->errstring=_T("You just interrupted and terminated the process."); // "TINTERRUPTEDBYUSER"
				return B_ERROR;
			}
		}
	}

	// Shell "sync" command, just as a precaution (launched outside app)
		if ( B_OK == get_ref_for_path("/bin/sync", &appRef)) {
			if (B_OK == be_roster->Launch(&appRef)){
				printf("Synced externally okay after cp\n");
			}
		}

	// Low level "sync".
//	sync();
	snooze(100000);

	// Second shell sync?
	if ( B_OK == get_ref_for_path("/bin/sync", &appRef)) {
		if (B_OK == be_roster->Launch(&appRef)){
		}
	}

	// unmount image file
	if (UnmountBFSImage(app->pathCV->GetDefaultMountPoint())<0) {
		app->errtype=_T("Error"); // "TERROR"
		app->errstring=app->pathCV->GetDefaultImageName();
		app->errstring<<"\n\n"<<_T("Could not unmount the file system."); // "TUNMOUNTERROR"
		app->HideStatus();
		free(args);
		return B_ERROR;
	}

	// Shell "sync" command, just as a precaution (launched outside app)
//	if ( B_OK == get_ref_for_path("/bin/sync", &appRef)) {
//		if (B_OK == be_roster->Launch(&appRef)){
//		}
//	}

	// Low level "sync".
	sync();
	snooze(100000);

		{
			BFile *fileRead = new BFile();
			fileRead->SetTo(filename, B_READ_ONLY);
			
			// Read beginning and end of file, to force a sync!!
			char tempBuffer[2];
			fileRead->Seek(0, SEEK_SET);
			fileRead->Read(tempBuffer, 1);
			fileRead->Seek(0, SEEK_END);
			fileRead->Read(tempBuffer, 1);

			fileRead->Unset();
			delete fileRead;
			fileRead = NULL;
		}


//		if ( B_OK == get_ref_for_path("/bin/sync", &appRef)) {
//			if (B_OK == be_roster->Launch(&appRef)){
//			}
//		}

		app->HideStatus();
		free(args);

	return B_OK;
}

off_t
GetBFSImageSize(const char* baseFolder)
{
	// Get size of directories and files
	off_t imgsize = 0;
	int32 files = 0;
	int32 directories = 0;

	{
		BDirectory baseDir(baseFolder);
		GetDirectorySize(baseDir, files, directories, imgsize);
			// This populates the variables 'files' and 'imgsize'
		baseDir.Unset();
	}

	// Add extra space for filesystem details.
	imgsize += files * 4096 + directories * 133200;	// must add the space need to store
																									// window positions and icon positions
																									// in the windows.
	const size_t FATsize = (5144*1024) + 64 * imgsize / (1024*1024)+384;
	imgsize += FATsize;
	
	return imgsize;
}

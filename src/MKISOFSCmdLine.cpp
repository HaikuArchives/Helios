#include <locale/Locale.h>
#include <locale/Catalog.h>

#include "Application1.h"
#include "ConfigFields.h"
#include "ErrorBox.h"
//#include "syslogprot.h"
#include "MKISOFSCmdLine.h"

// extern SyslogProt	*SYSLOG;

#define B_TRANSLATION_CONTEXT "mkisofs"

status_t MKISOFS_CommandLine(const char** argv, int* argc, bool printsize, const char* outputFile)
{
	Application1* app = (Application1*)be_app;
	Window1* window1 = (Window1*)app->window1;
	View1* view1 = (View1*)window1->view1;
	int32 start_sec, end_sec;
	BString basefolder = app->base_folder;

	argv[(*argc)++] = strdup(app->CM->GetString(PATH_TO_MKISOFS)->String());
	if (printsize) {
		argv[(*argc)++] = strdup("-print-size");
		argv[(*argc)++] = strdup("-gui");
	}

	// follow links in the project folder
	argv[(*argc)++] = strdup("-follow-links");
	//	// Rock Ridge extensions
	//	argv[(*argc)++]=strdup("-R");
	// application name
	if (strlen(app->imageCV->GetApplication()) > 0) {
		argv[(*argc)++] = strdup("-A");
		argv[(*argc)++] = strdup(app->imageCV->GetApplication());
	}
	// preparer name
	if (strlen(app->imageCV->GetPreparer()) > 0) {
		argv[(*argc)++] = strdup("-p");
		argv[(*argc)++] = strdup(app->imageCV->GetPreparer());
	}
	// publisher description
	if (strlen(app->imageCV->GetPublisher()) > 0) {
		argv[(*argc)++] = strdup("-P");
		argv[(*argc)++] = strdup(app->imageCV->GetPublisher());
	}

	switch (view1->CDType()) {

	///////////////////
	// D A T A - C D //
	///////////////////

	case DATACD_INDEX: {
		// volume name
		argv[(*argc)++] = strdup("-V");
		argv[(*argc)++] = strdup(view1->GetVolumeName());

		// BList	*list=app->fspanelCV->GetArgumentList();
		BList* list = view1->datapanel->GetFilesystem();

		for (int i = 0; i < list->CountItems(); i++) {
			argv[(*argc)++] = strdup(((BString*)(list->ItemAt(i)))->String());
			printf("%s ", ((BString*)(list->ItemAt(i)))->String());
		}
		for (int i = list->CountItems() - 1; i >= 0; i--) {
			delete (BString*)(list->RemoveItem(i));
		}
		printf("\n");

		delete list;

		// iso file system
		//			if (strlen(view1->datapanel->GetFileSystem())>0)
		//				argv[(*argc)++] = strdup(view1->datapanel->GetFileSystem());
		//			// GetFileSystem() only returns ONE arg. so add the missing
		//			// parameter when joliet fs has been chosen.
		//			if (strcmp(view1->datapanel->GetFileSystem(),"-J")==0)
		//				argv[(*argc)++] = strdup("-l");
		// multisession parameters
		if ((view1->IsMultisession()) && (!printsize)) {
			// ask whether this session should be the first, last or another
			// session of the disc
			ErrorBox* alert =
				new ErrorBox(E_ORANGE_COLOR, "", B_TRANSLATE("Is this session going to be the first, the "
												 "last or another session of a multisession "
												 "disc?"), // "MULTISESSION_ALERT"
							 B_TRANSLATE("another"),					  // "MULTISESSION_ALERT_B0"
							 B_TRANSLATE("last"),					  // "MULTISESSION_ALERT_B1"
							 B_TRANSLATE("first"));					  // "MULTISESSION_ALERT_B2"
			int selected = alert->Go();

			if (selected != 2)
				if (app->GetMultisessionSectors(start_sec, end_sec) != B_OK) {
					app->errtype = B_TRANSLATE("Error"); // "TERROR"
					app->errstring =
						B_TRANSLATE("Could not read inserted multisession disc!"); // "MULTISESSION_FAILS_ALERT"
					return B_ERROR;
				}
			BString text = "";
			if (selected == 0) { // it is another session!
				argv[(*argc)++] = strdup("-M");
				argv[(*argc)++] = strdup(app->deviceCV->GetWriterDeviceNumber());
				argv[(*argc)++] = strdup("-C");
				text << start_sec << "," << end_sec;
				argv[(*argc)++] = strdup(text.String());
			}
			if (selected == 1) { // it is the last session!
				argv[(*argc)++] = strdup("-M");
				argv[(*argc)++] = strdup(app->deviceCV->GetWriterDeviceNumber());
				argv[(*argc)++] = strdup("-C");
				text << start_sec << "," << end_sec;
				argv[(*argc)++] = strdup(text.String());
			}
		} // multisession

		break;
	}

	/////////////////////
	// A U D I O - C D //
	/////////////////////

	case AUDIOCD_INDEX: {
		// will never be here!
		break;
	}

	///////////////////////////
	// B O O T A B L E - C D //
	///////////////////////////

	case BOOTABLECD_INDEX: {

		// volume name
		argv[(*argc)++] = strdup("-V");
		argv[(*argc)++] = strdup(view1->GetVolumeNameBootCD());

		// BList	*list=app->fspanelCV->GetArgumentList();
		BList* list = view1->bootablepanel->GetFilesystem();

		for (int i = 0; i < list->CountItems(); i++) {
			argv[(*argc)++] = strdup(((BString*)(list->ItemAt(i)))->String());
		}
		for (int i = list->CountItems() - 1; i >= 0; i--) {
			delete (BString*)(list->RemoveItem(i));
		}

		delete list;

		//			// iso file system
		//			if (strlen(view1->bootablepanel->GetFileSystem())>0)
		//				argv[(*argc)++] = strdup(view1->bootablepanel->GetFileSystem());
		//			// GetFileSystem() only returns ONE arg. so add the missing
		//			// parameter when joliet fs has been chosen.
		//			if (strcmp(view1->bootablepanel->GetFileSystem(),"-J")==0)
		//				argv[(*argc)++] = strdup("-l");

		BString bootimage = view1->GetImageNameBootCD();
		bootimage.RemoveFirst("/");
		// add where to find the boot image file
		argv[(*argc)++] = strdup("-b");
		argv[(*argc)++] = strdup(bootimage.String());
		// and where the boot catalog can be found

		// if there is a dot in the image file name, cut off the
		// string after the dot and add "catalog"
		// the dot has to be after the last slash
		if (bootimage.FindLast(".") > bootimage.FindLast("/")) {
			bootimage.Remove(bootimage.FindLast("."), bootimage.Length() - bootimage.FindLast("."));
		}
		bootimage.Append(".cat");

		argv[(*argc)++] = strdup("-c");
		argv[(*argc)++] = strdup(bootimage.String());
		break;
	}

	/////////////////////
	// C D - E X T R A //
	/////////////////////

	case CDEXTRA_INDEX: {
		BString text = "";

		// volume name
		argv[(*argc)++] = strdup("-V");
		argv[(*argc)++] = strdup(view1->GetVolumeNameCDExtra());
		basefolder = view1->GetImageNameCDExtra();
		if (!printsize) {
			//				argv[(*argc)++] = strdup("-C");
			//				if (app->GetMultisessionSectors(start_sec, end_sec)!=B_OK) {
			//					app->errtype=B_TRANSLATE("Error"); // "TERROR"
			//					app->errstring=B_TRANSLATE("Could not read inserted multisession disc!"); //
			//"MULTISESSION_FAILS_ALERT"
			//					return B_ERROR;
			//				}
			//				text << start_sec << "," << end_sec;
			//				argv[(*argc)++] = strdup(text.String());
		}

		// BList	*list=app->fspanelCV->GetArgumentList();
		BList* list = view1->cdextrapanel->GetFilesystem();

		for (int i = 0; i < list->CountItems(); i++) {
			argv[(*argc)++] = strdup(((BString*)(list->ItemAt(i)))->String());
		}
		for (int i = list->CountItems() - 1; i >= 0; i--) {
			delete (BString*)(list->RemoveItem(i));
		}

		delete list;

		break;
	}

#ifdef INCLUDE_DVD
	case DVDVIDEO_INDEX: {
		// volume name
		argv[(*argc)++] = strdup("-V");
		argv[(*argc)++] = strdup(view1->GetVolumeNameDVDVideo());

		// BList	*list=app->fspanelCV->GetArgumentList();
		BList* list = view1->dvdvideopanel->GetFilesystem();

		for (int i = 0; i < list->CountItems(); i++) {
			argv[(*argc)++] = strdup(((BString*)(list->ItemAt(i)))->String());
		}
		for (int i = list->CountItems() - 1; i >= 0; i--) {
			delete (BString*)(list->RemoveItem(i));
		}

		delete list;

		//			// iso file system
		//			if (strlen(view1->dvdvideopanel->GetFileSystem())>0)
		//				argv[(*argc)++] = strdup(view1->dvdvideopanel->GetFileSystem());
		//			// GetFileSystem() only returns ONE arg. so add the missing
		//			// parameter when joliet fs has been chosen.
		//			if (strcmp(view1->dvdvideopanel->GetFileSystem(),"-J")==0)
		//				argv[(*argc)++] = strdup("-l");
		break;
	}
#endif
	default: {
		break;
	}
	}

	// add output file name
	if ((outputFile != NULL) && (!printsize)) {
		argv[(*argc)++] = strdup("-o");
		argv[(*argc)++] = strdup(outputFile);
	}

	argv[(*argc)++] = strdup(basefolder.String());
	argv[(*argc)] = NULL;

	// { DEBUG
	/*	char debug[1024];
		sprintf(debug, "Helios|MKISOFSCmdLine.cpp::MKISOFS_CommandLine()|");
		for (int i=0; argv[i]!=NULL; i++)
			sprintf(debug, "%s %s", strdup(debug), argv[i]);
		SYSLOG->SendSyslogMessage(1, 7, debug);
	*/ // } DEBUG

	return B_OK;
}

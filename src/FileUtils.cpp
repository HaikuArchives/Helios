// File Utilities - Implementation
#include "FileUtils.h"

#include <Directory.h>
#include <fs_attr.h>
#include <InterfaceKit.h>
#include <Query.h>
#include <StopWatch.h>
#include <Volume.h>
#include <fs_volume.h>

#include "Application1.h"
#include "CDRECORDCmdLine.h"
#include "DebugTools.h"
#include "ErrorBox.h"
#include "FileChooser.h"
#include "MString.h"
#include "PipeCmd.h"

#include <locale/Locale.h>
#include <locale/Catalog.h>

#define B_TRANSLATION_CONTEXT "File utils"

// copies file "source" to "destination"
// destination file will be overwritten if it exists already.
bool FileCopy(const char* source, const char* destination, size_t srcfilesize)
{
	bool retval = true;

	BFile* src = new BFile(source, B_READ_ONLY);
	BFile* dst = new BFile(destination, B_READ_WRITE | B_CREATE_FILE);
	if ((src->InitCheck() == B_OK) && (dst->InitCheck() == B_OK)) {

		Application1* app = dynamic_cast<Application1*>(be_app);

		BStopWatch* watch = new BStopWatch("MBps", true);

		BString tmpstring;
		tmpstring << B_TRANSLATE("Reading CD") << B_UTF8_ELLIPSIS; // "L:Reading CD..."
		app->ShowStatus(tmpstring.String(), -1);
		app->SetStatusInfo("--- MB/s");

		char info[20];
		char* buffer = new char[512 * 1024];
		int buffersize = 64 * 1024;
		const int sectorsize = 2048;

		dst->SetSize(0);
		ssize_t numread = src->Read(buffer, buffersize);
		float progressed = 0, sum = 0;
		while ((dst->Write(buffer, numread) == numread) && (buffersize >= sectorsize)) {
			numread = src->Read(buffer, buffersize);
			if ((numread <= 0) && (buffersize > sectorsize)) {
				buffersize /= 2;
				printf("HELIOS: Reducing Buffer Size To %d Bytes.\n", buffersize);
			}
			if (buffersize <= sectorsize) {
				buffersize = 0;
			}
			if (app->fStatusWindow->interrupted) {
				retval = false;
				break;
			}
			progressed += numread;
			if (progressed > 64 * 1024 * 8) {
				sprintf(info, "%0.1f MB/s",
						((float)progressed) / ((float)watch->ElapsedTime() * 1.048576));
				app->SetStatusInfo(info);
				watch->Reset();
				sum += progressed;
				app->SetPercentage(100 * sum / srcfilesize);
				progressed = 0.0F;
			}
		}

		delete watch;
		watch = NULL;
		delete[] buffer;
		buffer = NULL;
	} else {
		retval = false;
	}

	dst->Sync();
	dst->Unset();
	src->Unset();

	delete src;
	delete dst;

	return retval;
}

bool FileExists(const char* filename)
{
	BEntry* entry = new BEntry(filename, true);
	const bool exists = entry->Exists();
	entry->Unset();
	delete entry;
	return exists;
}

// creates a new folder named "folder_name"
// return value: "true" if successful, "false" if not
bool CreateFolder(const char* folder_name)
{
	BDirectory* dir = new BDirectory();
	const status_t createdStatus = dir->CreateDirectory(folder_name, NULL);
	dir->Unset();
	delete dir;
	return (createdStatus == B_OK);
}

// fügt rekursiv den ordner/die datei hinzu
bool CreateEntry(char* pathname, char* entryname)
{
	//+MSH: TODO - replace dependency on be_app.
	Application1* app = dynamic_cast<Application1*>(be_app);

	static long depthCount = 0;
	static bool haveEncounteredProjectFolder = false;
	depthCount++;

	// Test explicitly for anything actually within the Project folders,
	// as this is NOT permitted.
	BEntry testEntry;
	testEntry.SetTo(pathname, false);
	BPath testPath;
	if (testEntry.GetPath(&testPath) == B_OK) {
		BString rootFolder = app->pathCV->GetProjectPath();
		RemoveTrailingSlash(rootFolder);
		BString testPathString = testPath.Path();
		testPathString.Truncate(rootFolder.Length());
		if (testPathString == rootFolder) {
			haveEncounteredProjectFolder = true;
		}
	}
	testEntry.Unset();
	testPath.Unset();

	if (!haveEncounteredProjectFolder) {
		BEntry entry;
		entry.SetTo(pathname, false); // links nicht nachverfolgen
		if (entry.IsDirectory()) {
			BDirectory dir;
			if (dir.SetTo(&entry) != B_OK) {
				entry.Unset();
				return false;
			}

			AppendTrailingSlash(&(app->cdir));
			app->cdir.Append(entryname);
			// printf("CDIR %s\n",cdir.String());

			if (app->filecounter % 20 == 0) {
				if (app->window1->Lock()) {
					BString tmpstring;
					tmpstring << B_TRANSLATE("Adding files") << B_UTF8_ELLIPSIS << " (" << app->filecounter
							  << ")"; // "L:Adding files..."
					app->window1->view1->statusBAR->SetText(tmpstring.String());
					app->window1->Unlock();
				}
			}
			app->filecounter++;

			if (dir.CreateDirectory(app->cdir.String(), NULL) != B_OK) {
				dir.Unset();
				entry.Unset();
				return false;
			}
			dir.Rewind();

			BEntry de;
			while (dir.GetNextEntry(&de, false) == B_OK) {
				BPath path;
				de.GetPath(&path);

				BString str = "";
				if (de.IsDirectory()) {
					AppendTrailingSlash(&(app->cdir));
					str.Append(app->cdir).Append(path.Leaf());
					// printf("CREATEENTRY %s\n",path.Path());

					// Recursive call to this function itself...
					CreateEntry((char*)path.Path(), (char*)path.Leaf());

					app->cdir.Remove(app->cdir.FindLast("/"),
									 app->cdir.Length() - app->cdir.FindLast("/"));
					// printf("PARENTDIR %s\n",cdir.String());
				}

				if (de.IsFile()) {
					str.Append(app->cdir).Append(path.Leaf());
					// printf("CREATEENTRY %s\n",path.Path());
					CreateEntry((char*)path.Path(), (char*)path.Leaf());
				}
				path.Unset();
				de.Unset();
			}
			dir.Unset();
		} else {
			if (entry.IsFile()) {

				if (app->filecounter % 20 == 0) {
					if (app->window1->Lock()) {
						BString tmpstring;
						tmpstring << B_TRANSLATE("Adding files") << B_UTF8_ELLIPSIS << " ("
								  << app->filecounter << ")"; // "L:Adding files..."
						app->window1->view1->statusBAR->SetText(tmpstring.String());
						app->window1->Unlock();
					}
				}
				app->filecounter++;

				BString str = app->cdir;
				AppendTrailingSlash(&str);

				{
					BPath path;
					entry.GetPath(&path);
					str.Append(path.Leaf());
					path.Unset();
				}

				{
					BSymLink* sl = new BSymLink(&entry);
					BDirectory dirSymLink;
					dirSymLink.CreateSymLink(str.String(), pathname, sl);
					dirSymLink.Unset();
					sl->Unset();
					delete sl;
				}

				// printf("str=\"%s\"\npathname=%s", str.String(), pathname);
				// str << "ln -s \"" << pathname << "\" \"" << cdir << "\"";
				// printf("%s\n",str.String());
				// system(str.String());
			}
		}
		entry.Unset();
	}

	depthCount--;
	if (depthCount == 0) {
		if (haveEncounteredProjectFolder) {
			haveEncounteredProjectFolder = false;
			ErrorBox* eb = new ErrorBox(E_RED_COLOR, B_TRANSLATE("Error"), // "TERROR"
										B_TRANSLATE("No files from within the Project folder can be added "
										"- any such files are ignored."), // "TFOUNDPROJECTFILE"
										B_TRANSLATE("Ok"));							 // "TOK"
			eb->Go();
		}
	}

	return true;
}

BBitmap* GetIcon(const char* path)
{
	BBitmap* icon = new BBitmap(BRect(0.0, 0.0, 15.0, 15.0), B_CMAP8);
	BEntry* entry = new BEntry(path, true);
	entry_ref r;
	// resolve link!!!
	entry->GetRef(&r);
	entry->Unset();
	delete entry;

	BNodeInfo::GetTrackerIcon(&r, icon, B_MINI_ICON);
	return icon;
}

BBitmap* GetIcon(entry_ref* ref)
{
	BBitmap* icon = new BBitmap(BRect(0.0, 0.0, 15.0, 15.0), B_CMAP8);
	BEntry* entry = new BEntry(ref, true);
	entry_ref r;
	// resolve link!!!
	entry->GetRef(&r);
	entry->Unset();
	delete entry;

	BNodeInfo::GetTrackerIcon(&r, icon, B_MINI_ICON);
	return icon;
}

int32 addFiles(void* data)
{
	//+MSH: TODO - replace dependency on be_app.
	Application1* app = dynamic_cast<Application1*>(be_app);

	BMessage* msg = new BMessage(*((BMessage*)data));
	delete (BMessage*)data;
	acquire_sem(app->fileadder_sem);

	app->SetWorking();

	///////////////////////////////////
	uint32 type;
	int32 count;
	off_t filesize = 0;
	char mp3name[250];
	bool goadding = true;
	BString current_folder = app->current_folder;
	int32 cdtype = app->window1->view1->CDType();

	app->filecounter = 0;

	msg->GetInfo("refs", &type, &count);
	if (type != B_REF_TYPE) {
		app->SetWorking(false);
		release_sem(app->fileadder_sem);
		return 0;
	}

	entry_ref ref;
	if (count == 1) {
		if (msg->FindRef("refs", &ref) == B_OK) {
			BNode* node = new BNode(&ref);
			BNodeInfo* info = new BNodeInfo(node);
			char type[B_FILE_NAME_LENGTH];

			info->GetType(type);
			delete info;
			node->Unset();
			delete node;

			// if a helios project has been dropped into the window
			// ask the user whether he want to add the file to the project
			// or open the dropped project.
			if (strcmp(type, HELIOSPROJECTMIMETYPE) == 0) {
				ErrorBox* eb =
					new ErrorBox(E_BLUE_COLOR, B_TRANSLATE("Project file dropped"), // "TPROJECTFILEDROPPED"
								 B_TRANSLATE("You just dropped a Helios project file. Do you want to add "
								 "this file to the current project or load this project with "
								 "Helios?"), // "TPROJECTFILEDROPPED_TEXT"
								 B_TRANSLATE("Load"),	// "TLOAD"
								 B_TRANSLATE("Add"));
				switch (eb->Go()) {
				case 0: { // LOAD PROJECT FILE
					BEntry* projectEntry = new BEntry(&ref);
					app->LoadProject(projectEntry);
					projectEntry->Unset();
					delete projectEntry;
					goadding = false;
					break;
				}

				case 1: { // ADD TO PROJECT
					goadding = true;
					break;
				}

				default: {
					goadding = true;
					break;
				}
				}
			}
		}
	}

	if (goadding) {
		for (int32 i = --count; i >= 0; i--) {
			if (msg->FindRef("refs", i, &ref) != B_OK) {
				if (app->window1->Alert("Error at FindRef()\n\nProbably you didn't drop a file.")) {
					app->SetWorking(false);
					release_sem(app->fileadder_sem);
					return 0;
				}
			} else {
				BEntry* entry = new BEntry();
				entry->SetTo(&ref, true);

				BPath path;
				if (entry->GetPath(&path) == B_OK) {
					sprintf(mp3name, "%s", path.Leaf());
					if (entry->GetSize(&filesize) == B_OK) {
						app->cdir = current_folder;
						// printf("current_folder=\"%s\"\n", current_folder.String());
						// printf("path.Path()=\"%s\"\npath.Leaf()=\"%s\"\n",path.Path(),
						// path.Leaf());
						CreateEntry((char*)path.Path(), (char*)path.Leaf());
						if ((cdtype == AUDIOCD_INDEX) || (cdtype == CDEXTRA_INDEX)) {
							float size;

							TrackLength* info = new TrackLength(new BEntry(path.Path(), true));
							size = info->GetBytes();
							if (size > 0) {
								if (app->window1->Lock()) {
									AudioRow* ar;
									app->window1->view1->listview1->AddRow(
										(ar = new AudioRow(
											 app->window1->view1->listview1->iCountItems() + 1,
											 path.Leaf(), 0, size, size / 176400)));
									ar->SetStartFrame((int64)0);
									ar->SetEndFrame((int64)(75.0F * (float)size / 176400.0F));
									ar->SetLength((int64)(75.0F * (float)size / 176400.0F));
									if (app->window1->view1->listview1->iCountItems() == 1)
										ar->SetPregap(150);
									app->window1->Unlock();
								}
							}
							delete info;
						}
					}
				}

				path.Unset();
				entry->Unset();
				delete entry;
			}

		} // for

	} // if (goadding)

	if (app->window1->Lock()) {
		BString tmpstring;
		tmpstring << B_TRANSLATE("Adding files") << B_UTF8_ELLIPSIS << " (" << app->filecounter
				  << ")"; // "L:Adding files..."
		app->window1->view1->statusBAR->SetText(tmpstring.String(), 4000000);
		app->window1->Unlock();
	}

	if (app->saveCV->AutoUpdate())
		be_app->PostMessage(UPDATEINFO_MSG);
	else
		app->ShowFolder();
	///////////////////////////////////

	delete msg;
	app->SetWorking(false);
	release_sem(app->fileadder_sem);
	return 0;
}

int32 removeFiles(void* data)
{
	//+MSH: TODO - replace dependency on be_app.
	Application1* app = dynamic_cast<Application1*>(be_app);

	BMessage* msg = new BMessage(*((BMessage*)data));
	delete (BMessage*)data;
	acquire_sem(app->fileremover_sem);

	if (app->window1->Lock()) {
		app->window1->view1->listview1->SetEditMode(true);
		app->window1->Unlock();
	}

	app->SetWorking();

	BString str = "", filename = "";
	int32 cdtype = app->window1->view1->CDType();
	BString current_folder = app->current_folder;
	BString base_folder = app->base_folder;

	BRow* row;
	BList list;

	if (app->window1->Lock()) {
		while ((row = app->window1->view1->listview1->CurrentSelection())) {
			if ((cdtype == AUDIOCD_INDEX) || (cdtype == CDEXTRA_INDEX))
				filename = ((AudioRow*)(row))->GetFilename();
			if (cdtype == DATACD_INDEX) filename = ((FolderRow*)(row))->GetFilename();
			if (cdtype == BOOTABLECD_INDEX) filename = ((FolderRow*)(row))->GetFilename();
			if (cdtype == DVDVIDEO_INDEX) filename = ((FolderRow*)(row))->GetFilename();
			if (filename.Compare("..") != 0) {
				str.SetTo(current_folder.String()).Append(filename.String());
				list.AddItem(new BString(filename));

				app->window1->view1->listview1->BColumnListView::RemoveRow(row);
				delete row;

				if ((cdtype == AUDIOCD_INDEX) || (cdtype == CDEXTRA_INDEX)) {
					for (int j = 0; j < app->window1->view1->listview1->iCountItems(); j++) {
						if (str.Compare(((AudioRow*)(app->window1->view1->listview1->RowAt(j)))
											->GetFilename()) == 0) {
							app->window1->view1->listview1->RemoveRow(j);
							delete app->window1->view1->listview1->RowAt(j);
							j = -1;
						}
					}
				}
			} else {
				app->window1->view1->listview1->Deselect(row);
				((BButton*)app->window1->view1->iconBAR->ChildViewAt("bar_deleteitem"))
					->SetEnabled(app->window1->view1->listview1->CurrentSelection() != NULL);
			}
		}
		app->window1->Unlock();
	}

	app->filecounter = 0;

	for (int32 k = list.CountItems() - 1; k >= 0; k--) {
		str.SetTo(current_folder.String()).Append(((BString*)list.ItemAt(k))->String());

		BEntry* fileEntry = new BEntry();
		if (fileEntry->SetTo(str.String(), false) == B_OK) {
			if (fileEntry->IsDirectory()) {
				app->DeleteAndCount(BDirectory(fileEntry));
			}

			if (cdtype == BOOTABLECD_INDEX) {
				BString cdpathtoimage = str;

				cdpathtoimage.RemoveFirst(base_folder);
				cdpathtoimage.Prepend("/");
				// going to remove the floppy boot image file...
				if (cdpathtoimage.Compare(app->window1->view1->bootablepanel->GetImageName()) ==
					0) {
					ErrorBox*
						eb =
							new ErrorBox(E_ORANGE_COLOR,
										 B_TRANSLATE("Delete floppy image?"), // "Error:Delete Floppy Image?"
										 B_TRANSLATE("If you click 'Yes' the path will be removed from "
										 "your project's settings. Click 'No' to keep the "
										 "file."), // "Error:Delete Floppy Image?Error text"
										 B_TRANSLATE("Yes"),
										 B_TRANSLATE("No"));
					if (eb->Go() == 1) {
						fileEntry->Unset();
						delete fileEntry;
						fileEntry = NULL;

						if (app->window1->Lock()) {
							app->window1->view1->listview1->Deselect(
								app->window1->view1->listview1->CurrentSelection());
							app->window1->Unlock();
						}
						continue;
					} else {
						if (app->window1->Lock()) {
							app->window1->view1->bootablepanel->SetImageName("");
							app->window1->Unlock();
						}
					}
				}
			}

			if (fileEntry->InitCheck() == B_OK) {
				fileEntry->Remove();
			}

			if (app->filecounter % 20 == 0) {
				if (app->window1->Lock()) {
					BString tmpstring;
					tmpstring << B_TRANSLATE("Removing files") << B_UTF8_ELLIPSIS << " (" << app->filecounter
							  << ")"; // "L:Removing files..."
					app->window1->view1->statusBAR->SetText(tmpstring.String());
					app->window1->Unlock();
				}
			}
			app->filecounter++;
		}

		fileEntry->Unset();
		delete fileEntry;
		fileEntry = NULL;

		str.SetTo(filename.String());
		delete (BString*)(list.RemoveItem(k));
	}

	if (app->window1->Lock()) {
		BString tmpstring;
		tmpstring << B_TRANSLATE("Removing files") << B_UTF8_ELLIPSIS << " (" << app->filecounter
				  << ")"; // "L:Removing files..."
		app->window1->view1->statusBAR->SetText(tmpstring.String(), 4000000);
		app->window1->Unlock();
	}
	app->UpdateTrackNumbers();

	if (app->saveCV->AutoUpdate()) {
		be_app->PostMessage(UPDATEINFO_MSG);
	}

	delete msg;
	app->SetWorking(false);
	if (app->window1->Lock()) {
		app->window1->view1->listview1->SetEditMode(false);
		app->window1->Unlock();
	}
	release_sem(app->fileremover_sem);
	return 0;
}

void GetDirectorySize(BDirectory& dir, int32& numFiles, int32& numDirs, off_t& totalSize)
{
	// MSH: Some inefficiencies with construction of objects here,
	// but this is to ensure entries and files are *truly* deleted
	// with each loop - part of the Helios "file sync" issue.
	const int32 c = dir.CountEntries();
	if (c > 0) {

		for (int32 i = 0; i < c; i++) {
			BEntry entry;
			dir.GetNextEntry(&entry, true);

			if (entry.IsDirectory()) {
				numDirs++;

				BDirectory nextDirectory(&entry);
				GetDirectorySize(nextDirectory, numFiles, numDirs, totalSize);
				nextDirectory.Unset();

			} else {
				numFiles++;

				off_t s = 0;
				attr_info info;

				BFile file;
				file.SetTo(&entry, B_READ_ONLY);

				char buf[B_ATTR_NAME_LENGTH];
				while (file.GetNextAttrName(buf) == B_OK) {
					file.GetAttrInfo(buf, &info);
					totalSize += info.size;
				}

				file.GetSize(&s);
				file.Unset();

				if (s % 2048 != 0) {
					totalSize += (int(s / 2048) + 1) * 2048;
				} else {
					totalSize += s;
				}
			}

			entry.Unset();
		}
	}
}

void DeleteAllIn(BDirectory& dir)
{
	const int32 c = dir.CountEntries();
	if (c > 0) {
		for (int32 i = 0; i < c; i++) {
			BEntry entry;
			dir.GetNextEntry(&entry, false);
			if (entry.IsDirectory()) {
				{
					BDirectory entryDir(&entry);
					DeleteAllIn(entryDir);
					entryDir.Unset();
				}
				entry.Remove();
			} else {
				entry.Remove();
			}
			entry.Unset();
		}
	}
	sync();
}

int MountBFSImage(const char* filename, const char* mountpoint)
{
	//+MSH: TODO - replace dependency on be_app.
	Application1* app = dynamic_cast<Application1*>(be_app);

	int timeout = app->applicationCV->GetTimeoutLength() / 5;
	int retvalue = 0;
	BString tmpstring;
	dev_t volume;

	tmpstring << B_TRANSLATE("Mounting image file") << B_UTF8_ELLIPSIS; // "TMOUNTING"
	app->SetStatus(tmpstring.String(), 0);

	sync();
#ifdef _BEOS_HAIKU_BUILD_
	while ((volume = fs_mount_volume(mountpoint, filename, "bfs", 0, NULL)) < B_OK) {
#else
	while ((retvalue = mount("bfs", mountpoint, filename, 2, NULL, 0)) < 0) {
#endif
		snooze(5000000);
		timeout--;
		if ((timeout == 0) || (app->fStatusWindow->interrupted)) {
			return -1;
		}
		tmpstring << B_TRANSLATE("Waiting") << B_UTF8_ELLIPSIS; // "TWAITING"
													   //		sprintf(info,"%s",B_TRANSLATE("TWAITING"));
		app->SetStatusInfo(tmpstring.String());
		app->SetPercentage((float)timeout * 100 /
						   ((float)app->applicationCV->GetTimeoutLength() / 5));
	};

	return retvalue;
}

int UnmountBFSImage(const char* mountpoint)
{
	//+MSH: TODO - replace dependency on be_app.
	Application1* app = dynamic_cast<Application1*>(be_app);

	int timeout = app->applicationCV->GetTimeoutLength();
	char info[40];
	int retvalue = 0;
	BString s;

	BString tmpstring;
	tmpstring << B_TRANSLATE("Unmounting Image file") << B_UTF8_ELLIPSIS; // "TUNMOUNTING"
	app->SetStatus(tmpstring.String(), 0);

	sync();
#ifdef _BEOS_HAIKU_BUILD_
	while ((retvalue = fs_unmount_volume(mountpoint, 0)) < 0) {
#else
	while ((retvalue = unmount(mountpoint)) < 0) {
#endif
		snooze(1000000);
		timeout--;
		if ((timeout == 0) || (app->fStatusWindow->interrupted)) {
			return -1;
		}
		sprintf(info, "%d\"", timeout);
		app->SetStatusInfo(info);
		app->SetPercentage((float)timeout * 100 / ((float)app->applicationCV->GetTimeoutLength()));
	};

	sync();
	s.SetTo("/bin/rmdir ").Append(mountpoint);
	system(s.String());

	return retvalue;
}

status_t BurnImage(const BString& filename)
{
	//+MSH: TODO - replace dependency on be_app.
	Application1* app = dynamic_cast<Application1*>(be_app);

	int arguments = 0;
	thread_id Thread;
	const char** args;
	BString str = "";
	BString device = "dev=";
	BString filesystem = "";
	BString fixate = "";
	BString tmpstring;
	int in, out, err;

	// check if file exists
	if (!FileExists(filename.String())) {
		app->errtype = B_TRANSLATE("TERROR");			// "TERROR"
		app->errstring = B_TRANSLATE("TNOTEXIST_ALERT"); // "TNOTEXIST_ALERT"
		return B_ERROR;
	}

	if (app->fStatusWindow->Lock()) {
		if (app->heliosCV->IsSimulation()) {
			app->fStatusWindow->SetTitle(
				B_TRANSLATE("Helios - SIMULATION")); // "TSTATUSWINDOWTITLESIMULATION"
		} else {
			app->fStatusWindow->SetTitle(B_TRANSLATE("Helios")); // "TSTATUSWINDOWTITLE"
		}
		app->fStatusWindow->Unlock();
	}
	tmpstring = "";
	tmpstring << B_TRANSLATE("Flushing buffers") << B_UTF8_ELLIPSIS; // "TFLUSHINGBUFFERS"
	app->ShowStatus(tmpstring.String(), 0);

	sync();

	tmpstring = "";
	tmpstring << B_TRANSLATE("Waiting for disc") << B_UTF8_ELLIPSIS; // "TWAITINGFORDISC"
	app->ShowStatus(tmpstring.String(), 0);

	// Check for image file size. Does it fit?
	if (app->window1->Lock()) {
		BFile* file = new BFile(filename.String(), B_READ_ONLY);
		if (file->InitCheck() == B_OK) {
			off_t filesize;

			file->GetSize(&filesize);
			app->window1->view1->box2->SetCDSize(filesize);
			file->Sync();
			file->Unset();

			if (!app->CheckCDSize()) {
				app->window1->Unlock();
				app->errtype = B_TRANSLATE("Error"); // "TERROR"
				app->errstring = B_TRANSLATE("The project data size is greater than the maximum size of a "
								 "CD. Remove some files or select 'Overburning' in the "
								 "preferences panel."); // "TOVERSIZED"
				delete file;
				return B_ERROR;
			}
		}
		delete file;
		app->window1->Unlock();
	}

	if (app->WaitForDisc(app->deviceCV->GetWriterDevicePath()) == B_TIMEOUT) {
		app->errtype = B_TRANSLATE("Timeout"); // "TTIMEOUTTYPE"
		app->errstring =
			B_TRANSLATE("The active process had to wait too long for an event."); // "TTIMEOUTERROR"
		app->HideStatus();
		return B_ERROR;
	}

	device << app->deviceCV->GetWriterDeviceNumber();
	str = "";
	args = (const char**)malloc(sizeof(char*) * (22));
	CDRECORD_CommandLine(args, &arguments, app->window1->view1->IsMultisession(),
						 false /*isStreaming*/);
	//	args[arguments++] = strdup(CM->GetString(PATH_TO_CDRECORD)->String());
	//	args[arguments++] = strdup(device.String());
	//	// set fifo-buffer size
	//	str.SetTo("fs=");
	//	str << heliosCV->GetFIFOSize() << "M";
	//	args[arguments++] = strdup(str.String());
	//	// set writer speed
	//	device.SetTo("-speed=");
	//	device << heliosCV->GetWriterSpeed();
	//	args[arguments++] = strdup(device.String());
	//	if (heliosCV->IsBurnproof()) args[arguments++]=strdup("driveropts=burnproof");
	//	if (!heliosCV->IsFixating()) args[arguments++] = strdup("-nofix");
	//	if (heliosCV->IsOverburning()) args[arguments++] = strdup("-overburn");
	//	if (heliosCV->IsDAO()) args[arguments++] = strdup("-dao");
	//	if (heliosCV->IsSimulation()) args[arguments++] = strdup("-dummy");
	//	if (window1->view1->IsMultisession()) args[arguments++] = strdup("-multi");
	args[arguments++] = strdup("-data");
	//	args[arguments++] = strdup("-v");
	args[arguments++] = strdup(filename.String());
	args[arguments] = NULL;

	printArguments("cdrecord non-streaming args", args);

	Thread = pipe_command(arguments, args, in, out, err);
	set_thread_priority(Thread, CDRECORD_PRIORITY);
	if (Thread < 0) {
		app->errtype = B_TRANSLATE("Error"); // "TERROR"
		app->errstring = "cdrecord\n\n";
		app->errstring << B_TRANSLATE("Could not load this command line tool. Make sure it is in the "
						  "correct place."); // "TCOULDNOTLOAD"
		app->HideStatus();
		free(args);
		return B_ERROR;
	}
	tmpstring = "";
	tmpstring << B_TRANSLATE("Waiting") << B_UTF8_ELLIPSIS; // "TWAITING"
	app->SetStatus(tmpstring.String(), Thread);
	rename_thread(Thread, CDRECORDTHREADNAME);
	resume_thread(Thread);

	app->ReadOutputOfCdrecord(out, Thread);

	close(in);
	close(out);
	close(err);

	app->fStatusWindow->Lock();
	app->fStatusWindow->SetTitle(B_TRANSLATE("Helios")); // "TSTATUSWINDOWTITLE"
	app->fStatusWindow->Unlock();
	app->HideStatus();
	free(args);

	if (app->fStatusWindow->interrupted) {
		app->errtype = B_TRANSLATE("Error"); // "TERROR"
		app->errstring =
			B_TRANSLATE("You just interrupted and terminated the process."); // "TINTERRUPTEDBYUSER"
		return B_ERROR;
	}

	if (app->deviceCV->IsWriterEjected()) {
		app->eject_media(app->deviceCV->GetWriterDevicePath());
	}

	return B_OK;
}

// return-value: zero-length-string=no file found or selected,
//			nonzero-length-string=path to file
const char* CheckCommandlineTool(const char* fullpath, const char* name)
{
	BString string, path;

	BFile cdrecord;
	if (cdrecord.SetTo(fullpath, B_READ_ONLY) != B_OK) {
		// cdrecord not found
		cdrecord.Unset();

		// "TXNOTFOUND"
		BString notFoundStr = B_TRANSLATE("NAME not found! It has to be installed to use Helios. Press "
							  "'Search' if you want Helios to search it for or press 'Cancel' "
							  "to exit.");
		notFoundStr = notFoundStr.Replace("NAME", name, 1 /*maxReplaceCount*/);

		BAlert* alert = new BAlert("alert", notFoundStr.String(), B_TRANSLATE("Cancel"), B_TRANSLATE("Search"));

		if (alert->Go() == 1) {
			// search for requested file and show the result to the user.
			// the user should now be able to choose the file out of a list.
			FileChooser* chooser = new FileChooser();
			BString query_string = "";
			BQuery* query = new BQuery();
			query->SetVolume(new BVolume(dev_for_path("/boot/")));
			query_string.Append("name = ");
			query_string.Append(name);
			query->SetPredicate(query_string.String());
			query->Fetch();

			BEntry entry;
			while (query->GetNextEntry(&entry, true) == B_OK) {
				if (entry.IsFile()) {
					chooser->AddFile(&entry);
				}
			}
			entry.Unset();

			delete query;
			string = strdup(chooser->Go());
			return strdup(string.String());
		}
		return strdup("");
	} else {
		cdrecord.Unset();
		return strdup(fullpath);
	}
}

void OutPutRAW(const char* filename, int fd)
{
	char* buffer(NULL);
	const int buffersize = 4 * 1024 * 1024;
	int64 framesdecoded = 0;

	printf("HELIOS: writing RAW audio: %s\n", filename);
	buffer = (char*)malloc(buffersize);

	BFile* infile = new BFile();
	infile->SetTo(filename, B_READ_ONLY);
	do {
		framesdecoded = infile->Read((void*)buffer, buffersize);
		write(fd, &buffer, framesdecoded);
	} while (framesdecoded > 0);
	free(buffer);
	infile->Unset();
	delete infile;
}

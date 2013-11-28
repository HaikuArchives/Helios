// File Utilities

#ifndef _FILE_UTILS_
#define _FILE_UTILS_

#include <Entry.h>
#include <File.h>
#include <Bitmap.h>

bool						FileCopy(const char *source, const char *destination, size_t srcfilesize);
bool						FileExists(const char *filename);
bool 						CreateFolder(const char *folder_name);
bool						CreateEntry(char *pathname, char *entryname);
BBitmap*				GetIcon(const char *path);
BBitmap*				GetIcon(entry_ref *ref);
int32 					addFiles(void *data);
int32						removeFiles(void *data);

void						GetDirectorySize(	BDirectory& dir,
																	int32& numFiles,
																	int32& numDirs,
																	off_t& totalSize);
									// Pass in:
									//	[in]dir					- The directory to measure the size of.
									//	[out]numFiles		- On exit, contains the total number of files in the directory.
									//	[out]numDirs		- On exit, contains the total number of directories, excluding the one passed in.
									//	[out]totalSize	- On exit, contains the total size of all files.

void						DeleteAllIn(BDirectory& dir);
int							UnmountBFSImage(const char *mountpoint);
int							MountBFSImage(const char *filename, const char *mountpoint);
status_t				BurnImage(const BString& filename);
const char *		CheckCommandlineTool(const char *fullpath, const char *name);
void						OutPutRAW(const char *filename, int fd);

#endif
// _FILE_UTILS_

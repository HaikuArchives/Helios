#ifndef _FILECHOOSER_H_
#define _FILECHOOSER_H_

#include <Window.h>
#include <View.h>
#include <File.h>
#include "ColumnListView.h"
#include <List.h>
#include <Button.h>
#include "FolderRow.h"
#include <Entry.h>
#include <FilePanel.h>

class FileChooser : public BWindow {
public:
	FileChooser();
	~FileChooser();
	
	void AddFile(const char *path);
	void AddFile(BEntry *file);
	
	const char *Go();

private:

	virtual void MessageReceived(BMessage *msg);

	BList			*fileList;	// contains files shown in the list
	BColumnListView	*listView;
	BView			*winView;	// the window's main view
	BButton		*okB,
				*cancelB,
				*browseB;
	sem_id			sem;
	BFilePanel		*filepanel;
	BString			filename;
};

#endif

#ifndef _FILEDIALOG_H_
#define _FILEDIALOG_H_

#include <Looper.h>
#include <Message.h>
#include <Entry.h>
#include <Window.h>

class FileDialog {
public:
	static BWindow	*fSourceWindow;
	static BWindow	*fmainWindow;
	static BEntry *OpenDialog(const char *title, const char *path=NULL, uint32 node_flavors=B_FILE_NODE);
	static BEntry *SaveDialog(const char *title, const char *path=NULL, const char *filename=NULL);
};







class DialogLooper : public BLooper {
public:
	DialogLooper();
	~DialogLooper();
	
	virtual void 	MessageReceived(BMessage *);
	BEntry			*GetEntry();
	
private:
	BEntry			*fEntry;
};

#endif


#ifndef _LOGSETTINGSWINDOW_H_			// change
#define _LOGSETTINGSWINDOW_H_			// change

#define _CLASSNAME	LogSettingsWindow		// change
#define _INHERITED		BWindow		// might be changed

#include <InterfaceKit.h>

class _CLASSNAME : public BWindow {
public:
	_CLASSNAME(BRect frame, const char *title);
	~_CLASSNAME();
	
	
	
	void	SaveSettings(BMessage *);
	void	LoadSettings(BMessage *);
	
protected:
	virtual void MessageReceived(BMessage *);
	virtual bool QuitRequested();

private:
	BView		*mainV;
	BCheckBox	*loggingCB;
};




#endif


#ifndef _CONFIGVIEW_H_
#define _CONFIGVIEW_H_

#include <View.h>

class ConfigView : public BView {
public:
	ConfigView(BRect frame);
	~ConfigView();
	
	void Invalidate();
	
	virtual void MessageReceived(BMessage *);
	
	virtual void GetSettings(BMessage *);
	virtual void SetSettings(BMessage *);
	
private:
	void AttachedToWindow();
};

#endif

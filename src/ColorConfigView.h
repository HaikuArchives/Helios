#ifndef _COLORCONFIGVIEW_H_
#define _COLORCONFIGVIEW_H_

#include "ConfigView.h"
#include <String.h>
#include <InterfaceKit.h>
#include "ColorTestView.h"

class ColorConfigView : public ConfigView
{
public:
	ColorConfigView(BRect frame);
	~ColorConfigView();

	rgb_color GetColor(int32 index);
	void SetColor(int32 index, rgb_color color);
	void AddItem(const char* text);

private:
	virtual void MessageReceived(BMessage*);
	virtual void AllAttached();

	BColorControl* mainCC;
	ColorTestView* testCTV;
	BListView* itemLV;
	BScrollView* itemSV;
	BButton* loaddefB;
};

#endif

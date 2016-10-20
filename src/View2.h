#ifndef _VIEW2_H_
#define _VIEW2_H_

#include <View.h>
#include <Bitmap.h>
#include <TextControl.h>
#include <TextView.h>
#include <Application.h>
#include <Button.h>

class View2 : public BView
{
public:
	View2();
	~View2(void);

	virtual void AttachedToWindow(void);
	BTextControl* textcontrol3;

private:
	BButton* button7;
	BButton* button8;
};

#endif

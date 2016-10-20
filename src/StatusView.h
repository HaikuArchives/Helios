#ifndef _STATUS_VIEW_
#define _STATUS_VIEW_

#include <View.h>
#include <Bitmap.h>
#include <StringView.h>
#include <Font.h>
#include <Application.h>
#include <Button.h>

#include "StatusBarView.h"

class StatusView : public BView
{
public:
	StatusView();
	~StatusView();

	BStringView* stringview12;
	BFont* font14;
	BButton* button17;
	BStatusBar* progressSB;
};

#endif
// _STATUS_VIEW_

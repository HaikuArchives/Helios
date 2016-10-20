#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str) gMSHLangMgr->_T(str).String()
#define _TPS(str) gMSHLangMgr->_T(str)
#else
#include <locale/Locale.h>
#endif

#include "ImageConfigView.h"

ImageConfigView::ImageConfigView(BRect frame) : ConfigView(frame)
{
	BFont* TCfont = new BFont(be_fixed_font);
	TCfont->SetSize(10.0);

	float max_width = 0.0;

	// TEXTCONTROLS
	publisherTC = new BTextControl(BRect(15, 8, frame.Width() - 15, 33), "icv_publisherTC",
								   _T("CD Publisher:"), // "TPUBLISHER"
								   "", NULL);
	// publisherTC->SetDivider(publisherTC->StringWidth(_T("CD Publisher:"))+5); // "TPUBLISHER"
	publisherTC->TextView()->SetMaxBytes(128);
	publisherTC->TextView()->SetFontAndColor(TCfont);
	AddChild(publisherTC);

	preparerTC = new BTextControl(BRect(15, 38, frame.Width() - 15, 63), "icv_preparerTC",
								  _T("CD Preparer:"), // "TPREPARER"
								  "", NULL);
	// preparerTC->SetDivider(preparerTC->StringWidth(_T("CD Preparer:"))+5); // "TPREPARER"
	preparerTC->TextView()->SetMaxBytes(128);
	preparerTC->TextView()->SetFontAndColor(TCfont);
	AddChild(preparerTC);

	applicationTC = new BTextControl(BRect(15, 68, frame.Width() - 15, 93), "icv_applicationTC",
									 _T("CD Application:"), // "TAPPLICATION"
									 "", NULL);
	// applicationTC->SetDivider(applicationTC->StringWidth(_T("CD Application:"))+5); //
	// "TAPPLICATION"
	applicationTC->TextView()->SetMaxBytes(128);
	applicationTC->TextView()->SetFontAndColor(TCfont);
	AddChild(applicationTC);

	/* calculate max divider */
	max_width = publisherTC->StringWidth(publisherTC->Label()) >
						preparerTC->StringWidth(preparerTC->Label()) ?
					max_width = publisherTC->StringWidth(publisherTC->Label()) :
					preparerTC->StringWidth(preparerTC->Label());

	max_width = max_width < applicationTC->StringWidth(applicationTC->Label()) ?
					applicationTC->StringWidth(applicationTC->Label()) :
					max_width += 0;

	max_width += 5;
	publisherTC->SetDivider(max_width);
	preparerTC->SetDivider(max_width);
	applicationTC->SetDivider(max_width);
}

ImageConfigView::~ImageConfigView()
{
}

const char* ImageConfigView::GetPublisher()
{
	return publisherTC->Text();
}

void ImageConfigView::SetPublisher(const char* str)
{
	publisherTC->SetText(str);
}

const char* ImageConfigView::GetPreparer()
{
	return preparerTC->Text();
}

void ImageConfigView::SetPreparer(const char* str)
{
	preparerTC->SetText(str);
}

const char* ImageConfigView::GetApplication()
{
	return applicationTC->Text();
}

void ImageConfigView::SetApplication(const char* str)
{
	applicationTC->SetText(str);
}

// PRIVATE

void ImageConfigView::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
	default: {
		BView::MessageReceived(msg);
		break;
	}
	}
}

#include "AboutWindow.h"
#include <GraphicsDefs.h>


AboutView::AboutView(BRect frame, const char *name, BBitmap *icon) :
		BView(frame, name, B_FOLLOW_ALL, B_WILL_DRAW)
{	
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	bIcon = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
}


AboutView::~AboutView()
{
}


void
AboutView::SetIcon(BBitmap *icon)
{
	bIcon->SetBits(icon->Bits(), icon->BitsLength(), 0, icon->ColorSpace());
	Invalidate();
}


void
AboutView::Draw(BRect updateRect)
{
	if (BRect(0, 0, (21 + 33), Bounds().Height()).Intersects(updateRect)) {
		SetDrawingMode(B_OP_OVER);
		rgb_color secondBackColor = ui_color( B_PANEL_BACKGROUND_COLOR );
		secondBackColor.red = ( (secondBackColor.red - 20) > 0) ? secondBackColor.red - 20 : 0;
		secondBackColor.green = ( (secondBackColor.green - 20) > 0) ? secondBackColor.green - 20 : 0;
		secondBackColor.blue = ( (secondBackColor.blue - 20) > 0) ? secondBackColor.blue - 20 : 0;
		SetHighColor(secondBackColor.red, secondBackColor.green, secondBackColor.blue, 255);
		FillRect(BRect(0, 0, (10+23), Bounds().Height()));
#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
		SetHighColor(tint_color(ui_color(B_PANEL_BACKGROUND_COLOR),B_DARKEN_3_TINT));
#else
		SetHighColor(ui_color(B_SHADOW_COLOR));
#endif
		DrawBitmap(bIcon, BPoint(21, 10));
	}
}


AboutWindow::AboutWindow(BRect frame, const char *title) :
		BWindow(frame, title, B_FLOATING_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL, B_NOT_ZOOMABLE | B_NOT_RESIZABLE)
{
	vAbout=new AboutView(Bounds(), "AboutView", NULL);	
	AddChild(vAbout);

	applicationNameSV = new BStringView(BRect(60, 1, (frame.Width() - 60), 27), "applicationNameSV", "");
	applicationNameSV->SetFont(be_bold_font);
	vAbout->AddChild(applicationNameSV);

	versionNumberSV=new BStringView(BRect(applicationNameSV->StringWidth(applicationNameSV->Text())+65, 1, frame.Width()-5, 27), "versionNumberSV", "");
	versionNumberSV->SetFontSize(10);
	vAbout->AddChild(versionNumberSV);

	copyrightStringSV=new BStringView(BRect(60,27, frame.Width()-20, 41), "copyrightStringSV", "");
	copyrightStringSV->SetFontSize(10);
	vAbout->AddChild(copyrightStringSV);

	textTV=new BTextView(BRect(60,55, frame.Width()-8-B_V_SCROLL_BAR_WIDTH, frame.Height()-7), "textTV",
			BRect(1,1,frame.Width()-83,400), B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW);
	textTV->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	textTV->MakeEditable(false);
	textTV->MakeSelectable(false);
	textTV->SetWordWrap(true);
	BFont	*font=new BFont(be_plain_font);
	font->SetSize(10);
	textTV->SetFontAndColor(font);

	textSV=new BScrollView("AboutBox:textSV", textTV, B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP_BOTTOM,
					0, false, true, B_NO_BORDER);

	vAbout->AddChild(textSV);
	delete font;
}


AboutWindow::~AboutWindow()
{
}


void
AboutWindow::SetApplicationName(const char *name)
{
	font_height	boldheight, plainheight;
	
	applicationNameSV->GetFontHeight(&boldheight);
	versionNumberSV->GetFontHeight(&plainheight);
	
	applicationNameSV->SetText(name);
	versionNumberSV->ResizeTo((Bounds().Width()-5)-(applicationNameSV->StringWidth(applicationNameSV->Text())+65), 26);
	versionNumberSV->MoveTo(applicationNameSV->StringWidth(applicationNameSV->Text())+65,
													applicationNameSV->Frame().top-(boldheight.descent-plainheight.descent));
}


void
AboutWindow::SetVersionNumber(const char *version)
{
	versionNumberSV->SetText(version);
}


void
AboutWindow::SetIcon(BBitmap *icon)
{
	vAbout->SetIcon(icon);
}


void
AboutWindow::SetCopyrightString(const char *copyright)
{
	copyrightStringSV->SetText(copyright);
}


void
AboutWindow::SetText(const char *text)
{
	textTV->SetText(text);
}


bool
AboutWindow::QuitRequested()
{
	Quit();
	return false;
}

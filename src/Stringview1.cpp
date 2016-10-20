#include "Stringview1.h"
#include <Application.h>
#include <NodeInfo.h>

void Stringview1::Draw(BRect updateRect)
{

	rgb_color lc = LowColor();
	rgb_color hc = HighColor();

	//	if (hasIcon) DrawBitmap(bmp,BRect(600,0,615,15));
	SetHighColor(0, 0, 0, 255);
	SetLowColor(255, 255, 255, 255);
	SetFontSize(10.0);
	DrawString(text->String(), BPoint(3, 12));
	DrawString(typetext->String(),
			   BPoint(Bounds().Width() - 20 - StringWidth(typetext->String()), 12));

	SetLowColor(lc);
	SetHighColor(hc);
	//	BView::Draw(updateRect);

	//	be_app->PostMessage('HL01');
}

Stringview1::Stringview1(BRect frame, const char* name, uint32 resizeMask, uint32 flags)
	: BView(frame, name, resizeMask, flags)
{

	hasIcon = false;
	bmp = new BBitmap(BRect(0, 0, 15, 15), B_CMAP8);
	typetext = new BString("");
	text = new BString("");
};

void Stringview1::SetText(const char* utext)
{

	rgb_color lc = LowColor();
	rgb_color hc = HighColor();

	SetHighColor(190, 190, 190, 255);
	FillRect(BRect(0, 0, StringWidth(text->String()) + 8, Bounds().Height()), B_SOLID_HIGH);
	SetHighColor(0, 0, 0, 255);
	SetLowColor(255, 255, 255, 255);
	text->SetTo(utext);
	DrawString(text->String(), BPoint(3, 12));

	SetLowColor(lc);
	SetHighColor(hc);
}

void Stringview1::SetTypeText(const char* utypetext)
{

	rgb_color lc = LowColor();
	rgb_color hc = HighColor();

	SetHighColor(190, 190, 190, 255);
	FillRect(BRect(Bounds().Width() - 20 - StringWidth(typetext->String()), 0,
				   Bounds().Width() - 20, Bounds().Height()),
			 B_SOLID_HIGH);
	SetHighColor(0, 0, 0, 255);
	SetLowColor(255, 255, 255, 255);
	typetext->SetTo(utypetext);
	DrawString(typetext->String(),
			   BPoint(Bounds().Width() - 20 - StringWidth(typetext->String()), 12));

	SetLowColor(lc);
	SetHighColor(hc);
}

void Stringview1::SetFileTypeText(BNode* unode)
{
	/*
		BNodeInfo *info=new BNodeInfo();
		char *mime=TUNKNOWNFILETYPE;
		BMimeType *mimetype=new BMimeType();

		if (info->SetTo(unode)==B_OK)
			if (info->GetType(mime)==B_OK)
				if (mimetype->SetTo(mime)==B_OK)
					{
					mimetype->GetShortDescription(mime);
					mimetype->Unset();
					}
		this->SetTypeText((const char *)mime);
		delete info;
		delete mimetype;
	*/
}

void Stringview1::SetIcon(BNode unode)
{
	/*
		BNodeInfo info;

		hasIcon=true;
		SetHighColor(190,190,190,255);
		FillRect(BRect(Bounds().Width()-20,0,Bounds().Width(),Bounds().Height()),B_SOLID_HIGH);
		if (info.SetTo(&unode)==B_OK)
			if (info.GetTrackerIcon(bmp, B_MINI_ICON)==B_OK)
				DrawBitmap(bmp,BRect(600,0,615,15)); else hasIcon=false;
	*/
}

void Stringview1::Clear()
{

	rgb_color lc = LowColor();
	rgb_color hc = HighColor();

	SetHighColor(190, 190, 190, 255);
	FillRect(Bounds(), B_SOLID_HIGH);
	hasIcon = false;
	text->SetTo("");
	typetext->SetTo("");

	SetLowColor(lc);
	SetHighColor(hc);
}

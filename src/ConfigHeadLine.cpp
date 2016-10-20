#include "ConfigHeadLine.h"

ConfigHeadLine::ConfigHeadLine(BRect frame, const char* name, uint32 resizingMode, uint32 flags)
	: BView(frame, name, resizingMode, flags)
{
	fText = "";
}

ConfigHeadLine::~ConfigHeadLine()
{
}

void ConfigHeadLine::SetText(const char* text)
{
	fText.SetTo(text);
	Draw(Bounds());
}

const char* ConfigHeadLine::Text()
{
	return fText.String();
}

void ConfigHeadLine::Draw(BRect updateRect)
{
	rgb_color highcolor = HighColor();

	SetHighColor(ViewColor());
	FillRect(Bounds());
	SetHighColor(highcolor);
	DrawString(fText.String(), BPoint(17, Bounds().Height() - 5));
	BView::Draw(updateRect);
}

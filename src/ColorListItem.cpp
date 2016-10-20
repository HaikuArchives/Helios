#include "ColorListItem.h"

const rgb_color EnabledSelectedTextColor = (rgb_color){0, 0, 0};
const rgb_color DisabledSelectedTextColor = (rgb_color){128, 128, 128};
const rgb_color EnabledDeselectedTextColor = (rgb_color){0, 0, 0};
const rgb_color DisabledDeselectedTextColor = (rgb_color){128, 128, 128};
const rgb_color EnabledSelectedBackgroundColor = (rgb_color){152, 152, 152};
const rgb_color DisabledSelectedBackgroundColor = (rgb_color){152, 152, 152};
const rgb_color EnabledDeselectedBackgroundColor = (rgb_color){255, 255, 255};
const rgb_color DisabledDeselectedBackgroundColor = (rgb_color){255, 255, 255};

ColorListItem::ColorListItem(const char* text, uint32 level, bool expanded)
	: BStringItem(text, level, expanded)
{

	SetFrameColor((rgb_color){0, 0, 0});
	SetColor((rgb_color){255, 255, 255});
	SetFrameSize(BRect(0, 0, 20, 10));
	SetFrameAlignment(B_ALIGN_LEFT);
}

ColorListItem::~ColorListItem()
{
}

void ColorListItem::SetFrameSize(BRect frame)
{
	fColorFrame = frame;
}

void ColorListItem::SetFrameAlignment(alignment align)
{
	fColorFrameAlign = align;
}

void ColorListItem::DrawItem(BView* owner, BRect itemRect, bool drawEverything)
{
	BRect textRect = itemRect;
	BRect colorRect = fColorFrame;

	colorRect.top = itemRect.top + 1;
	colorRect.bottom = itemRect.bottom - 1;
	textRect.right = textRect.right - (fColorFrame.Width());
	colorRect.left++;
	colorRect.OffsetBy(textRect.Width(), 0);

	switch (fColorFrameAlign) {
	case B_ALIGN_LEFT: {
		textRect.OffsetBy(fColorFrame.Width(), 0);
		colorRect.OffsetBy(-textRect.Width(), 0);
		break;
	}
	default:
		break;
	}

	// center color rect vertically
	// colorRect.OffsetTo(colorRect.left, (itemRect.Height()-colorRect.Height())/2);

	if (IsEnabled()) {
		if (IsSelected()) {
			owner->SetHighColor(EnabledSelectedBackgroundColor);
		} else {
			owner->SetHighColor(EnabledDeselectedBackgroundColor);
		}
	} else {
		if (IsSelected()) {
			owner->SetHighColor(DisabledSelectedBackgroundColor);
		} else {
			owner->SetHighColor(DisabledDeselectedBackgroundColor);
		}
	}

	owner->FillRect(itemRect);

	owner->SetHighColor(fFrameColor);
	owner->StrokeRect(colorRect);

	colorRect.InsetBy(1, 1);
	owner->SetHighColor(fColor);
	owner->FillRect(colorRect);

	if (IsEnabled()) {
		if (IsSelected()) {
			owner->SetHighColor(EnabledSelectedTextColor);
			owner->SetLowColor(EnabledSelectedBackgroundColor);
		} else {
			owner->SetHighColor(EnabledDeselectedTextColor);
			owner->SetLowColor(EnabledDeselectedBackgroundColor);
		}
	} else {
		if (IsSelected()) {
			owner->SetHighColor(DisabledSelectedTextColor);
			owner->SetLowColor(DisabledSelectedBackgroundColor);
		} else {
			owner->SetHighColor(DisabledDeselectedTextColor);
			owner->SetLowColor(DisabledDeselectedBackgroundColor);
		}
	}

	font_height fh;

	owner->GetFontHeight(&fh);
	owner->DrawString(Text(), BPoint(textRect.left + 3, textRect.bottom - fh.descent));
	// BStringItem::DrawItem(owner, itemRect, drawEverything);
}

void ColorListItem::SetColor(rgb_color color)
{
	fColor = color;
}

rgb_color ColorListItem::GetColor() const
{
	return fColor;
}

void ColorListItem::SetFrameColor(rgb_color color)
{
	fFrameColor = color;
}

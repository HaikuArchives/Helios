#include "ColorTestView.h"

ColorTestView::ColorTestView(BRect frame, const char *name)
			: BView(frame, name, B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW) {
	fColor=(rgb_color){0,0,0,0};
}

ColorTestView::~ColorTestView() {
}

void	ColorTestView::Draw(BRect updateRect) {
	BRect	bnds=Bounds();
	
	//BView::Draw(updateRect);
	SetHighColor(184, 184, 184);		// dark gray
	StrokeLine(BPoint(0,0), BPoint(bnds.right, 0));
	StrokeLine(BPoint(0,0), BPoint(0, bnds.bottom));
	SetHighColor(96, 96, 96);		// darker gray
	StrokeLine(BPoint(1,1), BPoint(bnds.right-1, 1));
	StrokeLine(BPoint(1,1), BPoint(1, bnds.bottom-1));
	SetHighColor(216, 216, 216);		// gray
	StrokeLine(BPoint(2,bnds.bottom-1), BPoint(bnds.right-1, bnds.bottom-1));
	StrokeLine(BPoint(bnds.right-1,2), BPoint(bnds.right-1, bnds.bottom-1));
	SetHighColor(255, 255, 255);		// white
	StrokeLine(BPoint(1,bnds.bottom), BPoint(bnds.right, bnds.bottom));
	StrokeLine(BPoint(bnds.right,1), BPoint(bnds.right, bnds.bottom));
	DrawColorArea();
}

void	ColorTestView::DrawColorArea() {
	BRect bnds=Bounds();
	
	bnds.InsetBy(2,2);
	SetHighColor(fColor);
	FillRect(bnds);
}

void	ColorTestView::SetTestColor(rgb_color color) {
	fColor=color;
	SetViewColor(fColor);
	DrawColorArea();
}

void	ColorTestView::MessageReceived(BMessage *msg) {
	switch(msg->what) {
		case CTV_COLOR_UPDATE_MSG: {
			break;
		}
		
		default: {
			BView::MessageReceived(msg);
			break;
		}
	}
}

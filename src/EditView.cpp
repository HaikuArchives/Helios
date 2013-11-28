#include "EditView.h"

EditView::EditView(BRect frame,
			const char *name,
			const char *text,
			BMessage *invokeMsg,
			uint32 resizingMode)
		: BView(frame, name, resizingMode, B_WILL_DRAW) {

	if (text)
		fText=text;
	if (invokeMsg)
		fInvocationMsg=invokeMsg;
	editTEV=new TextEditView(this);
	fAlignment=B_ALIGN_LEFT;
	SetHighColor(0,0,0);
	SetLowColor(255,255,255);
}

EditView::~EditView() {
}
	
void		EditView::SetText(const char *text) {
	fText=text;
	Draw(BRect());
}

const char 	*EditView::Text() {
	return fText.String();
}

void		EditView::SetInvocationMessage(BMessage *msg) {
	if (fInvocationMsg)
		delete fInvocationMsg;
	fInvocationMsg=msg;
}

void	EditView::SetAlignment(alignment align) {
	fAlignment=align;
	Draw(BRect());
}

alignment	EditView::Alignment() {
	return fAlignment;
}

void	EditView::MouseDown(BPoint point) {
	rgb_color	color=HighColor();
	
	editTEV->SetText(fText.String());
	editTEV->SetViewColor(ViewColor());
//	editTEV->SetHighColor(HighColor());
	
	// set font
	BFont	*font=new BFont();
	GetFont(font);
	editTEV->SetFontAndColor(font, B_FONT_ALL, &color);
	
	editTEV->SetAlignment(Alignment());

	AddChild(editTEV);
}

void	EditView::Draw(BRect updateRect) {
	Window()->BeginViewTransaction();
	
	rgb_color	fHighColor=HighColor();
	rgb_color	fLowColor=LowColor();
	BPoint		point;
	
	SetHighColor(ViewColor());
	FillRect(Bounds());
	SetHighColor(fHighColor);
	switch(fAlignment) {
		case B_ALIGN_LEFT: {
			point=BPoint(1, Bounds().bottom-5);
			break;
		}
		case B_ALIGN_RIGHT: {
			point=BPoint(Bounds().Width()-StringWidth(fText.String())-1, Bounds().bottom-5);
			break;
		}
		case B_ALIGN_CENTER: {
			point=BPoint((Bounds().Width()-StringWidth(fText.String()))/2, Bounds().bottom-5);
			break;
		}
	}
	DrawString(fText.String(), point);
	Window()->EndViewTransaction();
}



TextEditView::TextEditView(EditView *parent)
		: BTextView(parent->Bounds(),
			"TextEditView",
			parent->Bounds(),
			B_FOLLOW_LEFT | B_FOLLOW_TOP,
			B_WILL_DRAW | B_FRAME_EVENTS | B_FULL_UPDATE_ON_RESIZE) {
	fParentView=parent;		
}

TextEditView::~TextEditView() {
}

void TextEditView::KeyDown(const char *bytes, int32 numBytes) {
	
	switch(bytes[0]) {
		case B_ESCAPE: {
			RemoveSelf();
			break;
		}
		case B_ENTER: {
			fParentView->SetText(Text());
			RemoveSelf();
			break;
		}
		default: {
			BTextView::KeyDown(bytes, numBytes);
			break;
		}
	}
}






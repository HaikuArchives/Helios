#include "IconBar.h"
#include <stdio.h>
#include <Resources.h>
#include <Application.h>

// IconBarSeparator

IconBarSeparator::IconBarSeparator(BRect frame, const char *name, uint32 resizingMode, uint32 flags)
	: BView(frame, name, resizingMode, flags) {
	SetViewColor(216, 216, 216);
}


void IconBarSeparator::Draw(BRect updateRect) {
	BRect	r=this->Bounds();
	
	SetHighColor(150, 150, 150);
	StrokeLine(BPoint(0, 2), BPoint(0, r.Height()-3));
	SetHighColor(255, 255, 255);
	StrokeLine(BPoint(1, 3), BPoint(1, r.Height()-2));
}

	
// IconButton

IconButton::IconButton(BRect frame, const char *name,
				BBitmap *icon, BMessage *message,
				uint32 resizingMode, uint32 flags)
	: BButton(frame, name, NULL, message, resizingMode, flags) {
	fIcon=icon;
	fBounds=frame;
}

IconButton::~IconButton() {
	if (fIcon)
		delete fIcon;
}

void IconButton::Draw(BRect updateRect) {
	DrawFrame(Value()!=0);
}

void IconButton::DrawFrame(bool pressed) {
	BRect	r=Bounds();
	rgb_color	bright=(rgb_color){255, 255, 255}, dark=(rgb_color){120,120,120};
	rgb_color	brightb=(rgb_color){216, 216, 216}, darkb=(rgb_color){190,190,190};
	float		left, top;
	char		i=(pressed?0:1);

	
	SetDrawingMode(B_OP_COPY);
	//frame
	SetHighColor(96, 96, 96);
	StrokeLine(BPoint(1,0), BPoint(r.right-1, 0));
	StrokeLine(BPoint(0,1), BPoint(0, r.bottom-1));
	StrokeLine(BPoint(r.right,1), BPoint(r.right, r.bottom-1));
	StrokeLine(BPoint(1,r.bottom), BPoint(r.right-1, r.bottom));
	
	SetHighColor(pressed?dark:bright);	
	StrokeLine(BPoint(1,1), BPoint(r.right-1, 1));
	StrokeLine(BPoint(1,1), BPoint(1, r.bottom-1));
	SetHighColor(pressed?darkb:dark);	
	StrokeLine(BPoint(r.right-1,2-i), BPoint(r.right-1, r.bottom-1));
	StrokeLine(BPoint(2-i,r.bottom-1), BPoint(r.right-1, r.bottom-1));

	SetHighColor((pressed || IsFocus())?darkb:brightb);	
	FillRect(r.InsetBySelf(2,2));

	SetDrawingMode(B_OP_ALPHA);
	left=(r.right-fIcon->Bounds().right)/2;
	top=(r.bottom-fIcon->Bounds().bottom)/2;
	DrawBitmap(fIcon, BPoint(pressed?left+1:left,pressed?top+1:top));
	
	if (!IsEnabled()) {
		SetDrawingMode(B_OP_BLEND);
		SetHighColor(216, 216, 216);
		FillRect(Bounds());
	}
}





// IconBar

IconBar::IconBar(BRect frame, const char *name, uint32 resizingMode, uint32 flags)
	: BView(frame, name, resizingMode, flags) {
	
	SetViewColor(216,216, 216);
	
	fButtonL=new BList();
}

IconBar::~IconBar() {
	delete fButtonL;
}

void IconBar::AddChildView(BView *button) {
	float	rightmost=5.0F;
	
	for (int32 i=0; i<fButtonL->CountItems(); i++) {
		rightmost+=((BView *)(fButtonL->ItemAt(i)))->Bounds().Width()+5;
	}
	fButtonL->AddItem(button);
	
	AddChild(button);
	button->MoveTo(rightmost, (Bounds().Height()-button->Bounds().Height())/2);
}

int32 IconBar::AddButton(const char *iconname, const char *name, BMessage *msg, uint32 resizingMode, bool miniicon) {
	BRect		rect, btnrect;
	const void	*ptr=NULL;
	size_t		size;
	
	if (miniicon) {
		rect.Set(0,0,15,15);
		btnrect.Set(0,0,24,22);
	} else {
		rect.Set(0,0,31,31);
		btnrect.Set(0,0,38, 38);
	}
	BBitmap	*bm=new BBitmap(rect, B_CMAP8);
	
	ptr=be_app->AppResources()->LoadResource(miniicon?'MICN':'ICON', iconname, &size);
	if (ptr)
		bm->SetBits(ptr, size, 0, B_CMAP8);	
	this->AddChildView(new IconButton(btnrect, name, bm, msg, resizingMode));
	return fButtonL->CountItems()-1;
}


int32 IconBar::AddButton(int32 ID, const char *name, BMessage *msg, uint32 resizingMode, bool miniicon) {
	BRect		rect, btnrect;
	const void	*ptr=NULL;
	size_t		size;
	
	if (miniicon) {
		rect.Set(0,0,15,15);
		btnrect.Set(0,0,24,22);
	} else {
		rect.Set(0,0,31,31);
		btnrect.Set(0,0,38, 38);
	}
	
	BBitmap *bm=new BBitmap(rect, B_CMAP8);
	ptr=be_app->AppResources()->LoadResource(miniicon?'MICN':'ICON', ID, &size);
	if (ptr)
		bm->SetBits(ptr, size, 0, B_CMAP8);	
	this->AddChildView(new IconButton(btnrect, name, bm, msg, resizingMode));
	return fButtonL->CountItems()-1;
}





int32 IconBar::AddButtonMSGG(const char *iconname, const char *name, BMessage *msg, uint32 resizingMode) {
	BRect			btnrect;
	const void	*ptr=NULL;
	size_t		size;
	
	
	BBitmap	*bm = NULL;
	BMessage 	*msgg;
	ptr=be_app->AppResources()->LoadResource('MSGG', iconname, &size);
	if (ptr) {
		msgg=new BMessage();
		msgg->Unflatten((const char *)ptr);
	//	bm=new BBitmap((BMessage *)ptr);
		bm=(BBitmap *)BBitmap::Instantiate(msgg);
		btnrect=bm->Bounds();
		btnrect.InsetBy(-3,-3);
		delete msgg;
		//bm->SetBits(ptr, size, 0, B_CMAP8);	
	}
	this->AddChildView(new IconButton(btnrect, name, bm, msg, resizingMode));
	return fButtonL->CountItems()-1;
}






BView *IconBar::RemoveChildView(const char *name) {
	BView	*btn;
	
	for (int32 i=0; i<fButtonL->CountItems(); i++) {
		if (strcmp ( (btn=(BView *)(fButtonL->ItemAt(i)))->Name(), name )==0) {
			RemoveChild(btn);
			return btn;
		}
	}
	return NULL;
}

BView *IconBar::ChildViewAt(int32 index) {
	return (BView *)fButtonL->ItemAt(index);
}

BView *IconBar::ChildViewAt(const char *name) {
	return FindView(name);
}

void IconBar::Draw(BRect updateRect) {
	BRect	r=this->Bounds();
	
	SetHighColor(255, 255, 255);
	StrokeLine(BPoint(0,0), BPoint(r.right-1, 0));
	StrokeLine(BPoint(0,1), BPoint(0, r.bottom-1));
	SetHighColor(150, 150, 150);
	StrokeLine(BPoint(r.right,0), BPoint(r.right, r.bottom));
	StrokeLine(BPoint(0,r.bottom), BPoint(r.right, r.bottom));
	SetHighColor(216, 216, 216);
	FillRect(r.InsetBySelf(1,1));
}

void IconBar::FrameResized(float width, float height) {
	Draw(Bounds());
}





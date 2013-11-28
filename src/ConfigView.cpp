#include "ConfigView.h"
#include "ConfigWindow.h"

#include <stdio.h>

ConfigView::ConfigView(BRect frame) :
	BView(BRect(0,0,frame.Width(),frame.Height()), "configview", B_FOLLOW_ALL, B_WILL_DRAW) {
}

ConfigView::~ConfigView() {
}

void ConfigView::AttachedToWindow() {
//	BRect frame = ( ( ConfigWindow* )Window() )->GetConfigViewFrame();
//	ResizeTo( frame.Width(), frame.Height() );
	SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );
}

void ConfigView::MessageReceived(BMessage *msg) {
	BView::MessageReceived(msg);
}

void ConfigView::Invalidate() {
	for (int32 i=0; ChildAt(i)!=NULL; i++) {
		ChildAt(i)->Invalidate();
	}
}

void ConfigView::GetSettings(BMessage *archive) {
}

void ConfigView::SetSettings(BMessage *archive) {
}

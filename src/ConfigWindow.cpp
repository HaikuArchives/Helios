#include <strings.h>

#include <Application.h>
#include <ListItem.h>

#include "Defines.h"
#include "ConfigWindow.h"

#define LV_SELECTION_CHANGED	'LVsl'

ConfigWindow::ConfigWindow(const char *title) : BWindow(BRect(0,0,600,300), title, B_TITLED_WINDOW, B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_ASYNCHRONOUS_CONTROLS) {

	MoveTo(100,100);
	viewsL=new BList();
	mainV=new BView(Bounds(), "mainV", B_FOLLOW_ALL, B_WILL_DRAW);
	mainV->SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );
	AddChild(mainV);
	
	selectionLV=new BOutlineListView(BRect(3,3,160,Bounds().bottom), "selectionLV", B_SINGLE_SELECTION_LIST, B_FOLLOW_LEFT | B_FOLLOW_TOP_BOTTOM);

	scrollSV=new BScrollView("scrollSV", selectionLV, B_FOLLOW_LEFT | B_FOLLOW_TOP_BOTTOM, 0, false, true, B_FANCY_BORDER);
	mainV->AddChild(scrollSV);
	
	BRect offsetRect = Bounds();
	offsetRect.left = scrollSV->Frame().right + 1;
	offsetRect.bottom = offsetRect.top + 20;

	labelCHL=new ConfigHeadLine(offsetRect, "cw_labelSV");
	mainV->AddChild(labelCHL);
	labelCHL->SetFontSize(12);
	labelCHL->SetFont(be_bold_font);
	labelCHL->SetViewColor(184,184,184);
	labelCHL->SetLowColor(184,184,184);

	offsetRect.top = labelCHL->Frame().bottom + 1;
	offsetRect.bottom = Bounds().bottom;
	
	boundingBOX=new BView(offsetRect, "containerview", B_FOLLOW_ALL, B_WILL_DRAW);
	mainV->AddChild(boundingBOX);
	
	selectionLV->MakeFocus(true);
	selectionLV->SetSelectionMessage(new BMessage(LV_SELECTION_CHANGED));
	
	RefreshConfigView();
}

ConfigWindow::~ConfigWindow() {
	for (int i=0; i<viewsL->CountItems(); i++) {
		((BView *)viewsL->ItemAt(i))->RemoveSelf();
		delete (BView *)viewsL->ItemAt(i);
	}
	delete viewsL;
}

void ConfigWindow::AddConfigView(const char *name, BView *view) {
	selectionLV->AddItem(new BStringItem(name, 1));
	viewsL->AddItem(view);
}

void ConfigWindow::AddTopic(const char *name) {
	selectionLV->AddItem((superLI=new BStringItem(name)));
	viewsL->AddItem(new ConfigView(GetConfigViewFrame()));
}

int32 ConfigWindow::GetConfigIndex(const char *name) {
	if (selectionLV->CountItems()>0)
		for (int32 i=0;i<selectionLV->CountItems(); i++) {
			if (strcmp(((BStringItem *)selectionLV->FullListItemAt(i))->Text(), name)==0) {
				return selectionLV->FullListIndexOf((BListItem *)selectionLV->FullListItemAt(i));
			}
		}
	return -1;
}

const char *ConfigWindow::GetConfigName(int32 index) {
	return strdup(((BStringItem *)selectionLV->FullListItemAt(index))->Text());
}


BView *ConfigWindow::GetConfigView(int32 index)  {
	return boundingBOX->ChildAt(index);
}

BRect ConfigWindow::GetConfigViewFrame() {
	return boundingBOX->Frame();
}

void ConfigWindow::Show() {
	if (selectionLV->CurrentSelection()<0) {
		selectionLV->Select(1);
	}
	SetWorkspaces(B_CURRENT_WORKSPACE);
	BWindow::Show();
}

bool ConfigWindow::SelectView(int32 index) {
	if (( selectionLV->CountItems() - 1 ) > index) {
		selectionLV->Select(index);
		RefreshConfigView();
		return true;
	} else
		return false;
}

/////////////////////
// PRIVATE METHODS //
/////////////////////


void ConfigWindow::ActivateView(int32 index) {
	labelCHL->SetText(GetConfigName(index));
}

void ConfigWindow::RefreshConfigView() {
	if (selectionLV->FullListCurrentSelection()>=0) {
		if (boundingBOX->CountChildren()>0)
			boundingBOX->RemoveChild(boundingBOX->ChildAt(0));
		if (selectionLV->FullListCurrentSelection()>=0) {
			boundingBOX->AddChild((BView *)(viewsL->ItemAt(selectionLV->FullListCurrentSelection())));
			ActivateView(selectionLV->FullListCurrentSelection());
		} else {
			labelCHL->SetText((const char *)NULL);
		}
	}	
}

void ConfigWindow::MessageReceived(BMessage *msg) {
	switch(msg->what) {
		case LV_SELECTION_CHANGED: {
			RefreshConfigView();
			break;
		}
		default: {
			BWindow::MessageReceived(msg);
			break;
		}
	}
}

bool ConfigWindow::QuitRequested() {
	Hide();
	be_app_messenger.SendMessage(SAVE_SETTINGS);
	return false;
}

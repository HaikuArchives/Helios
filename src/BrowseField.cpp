#include "BrowseField.h"

#define BROWSEBUTTON_MSG 'BrF0'

BrowseField::BrowseField(BRect frame, const char* name, uint32 resizingMode, uint32 flags)
	: BView(frame, name, resizingMode, flags)
{

	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	fileTC = new BTextControl(BRect(0, 0, 10, 10), "BrowseField_TextControl", "", "", NULL,
							  B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP_BOTTOM);
	browseB = new BButton(BRect(0, 0, 10, 10), "BrowseField_Button", "",
						  new BMessage(BROWSEBUTTON_MSG), B_FOLLOW_TOP_BOTTOM | B_FOLLOW_RIGHT);

	AddChild(fileTC);
	AddChild(browseB);
}

BrowseField::~BrowseField()
{
}

void BrowseField::Init(const char* title, const char* path, uint32 node_flavors)
{
	if (title == NULL)
		fTitle = "";
	else
		fTitle = title;
	if (path == NULL)
		fPath = "";
	else
		fPath = path;
	fNodeFlavors = node_flavors;
}

BButton* BrowseField::GetButton()
{
	return browseB;
}

BTextControl* BrowseField::GetTextControl()
{
	return fileTC;
}

// PROTECTED

void BrowseField::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
	case BROWSEBUTTON_MSG: {
		BEntry* entry;
		BPath path;
		FileDialog::fSourceWindow = this->Window();
		fPath.SetTo(fileTC->Text());
		if (fPath.FindLast("/") >= 0)
			fPath.Remove(fPath.FindLast("/"), fPath.Length() - fPath.FindLast("/"));
		entry = FileDialog::OpenDialog(fTitle.String(), fPath.String(), fNodeFlavors);
		if (entry->InitCheck() == B_OK) {
			entry->GetPath(&path);
			fileTC->SetText(path.Path());
		}
		break;
	}
	default: {
		BView::MessageReceived(msg);
	}
	}
}

void BrowseField::AttachedToWindow()
{
	BView::AttachedToWindow();

	BRect frame = Bounds();
	float pBW, pBH;

	browseB->GetPreferredSize(&pBW, &pBH);
	browseB->ResizeTo(pBW, pBH);
	browseB->MoveTo(frame.right - (pBW), (frame.Height() - browseB->Frame().Height()) / 2);

	fileTC->GetPreferredSize(&pBW, &pBH);
	fileTC->ResizeTo(frame.Width() - (browseB->Frame().Width() + 5), pBH);
	fileTC->MoveTo(0, (frame.Height() - pBH) / 2);

	browseB->SetTarget(this);
}

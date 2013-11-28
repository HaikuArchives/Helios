#include "VersionConfigView.h"

VersionConfigView::VersionConfigView(BRect frame) :
	ConfigView(frame)
{
	cdda2wavVC=new VersionControl(BRect(15, 5, frame.Width()-15, 21), "vcv_cdda2wavVC", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP, B_WILL_DRAW);
	AddChild(cdda2wavVC);
	cdrecordVC=new VersionControl(BRect(15, 25, frame.Width()-15, 41), "vcv_cdrecordVC", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP, B_WILL_DRAW);
	AddChild(cdrecordVC);
	heliosVC=new VersionControl(BRect(15, 45, frame.Width()-15, 61), "vcv_heliosVC", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP, B_WILL_DRAW);
	AddChild(heliosVC);
	mkisofsVC=new VersionControl(BRect(15, 65, frame.Width()-15, 81), "vcv_mkisofsVC", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP, B_WILL_DRAW);
	AddChild(mkisofsVC);
	readcdVC=new VersionControl(BRect(15, 85, frame.Width()-15, 101), "vcv_readcdVC", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP, B_WILL_DRAW);
	AddChild(readcdVC);
}

VersionConfigView::~VersionConfigView() {
}


void	VersionConfigView::SetHeliosVersion(const char *version, BBitmap *icon) {
	heliosVC->SetIcon(icon);
	heliosVC->SetText(version);
}

void	VersionConfigView::SetCDRecordVersion(const char *version, BBitmap *icon) {
	cdrecordVC->SetIcon(icon);
	cdrecordVC->SetText(version);
}

void	VersionConfigView::SetMkISOfsVersion(const char *version, BBitmap *icon) {
	mkisofsVC->SetIcon(icon);
	mkisofsVC->SetText(version);
}

void	VersionConfigView::SetReadCDVersion(const char *version, BBitmap *icon) {
	readcdVC->SetIcon(icon);
	readcdVC->SetText(version);
}

void	VersionConfigView::SetCDDA2WAVVersion(const char *version, BBitmap *icon) {
	cdda2wavVC->SetIcon(icon);
	cdda2wavVC->SetText(version);
}


// PRIVATE

void	VersionConfigView::MessageReceived(BMessage *msg) {
	switch(msg->what) {
		default: {
			BView::MessageReceived(msg);
			break;
		}
	}
}

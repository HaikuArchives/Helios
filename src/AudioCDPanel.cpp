#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str)	gMSHLangMgr->_T(str).String()
#define _TPS(str)	gMSHLangMgr->_T(str)
#else
#include <locale/Locale.h>
#endif

#include "AudioCDPanel.h"

#define CDTEXTBOX_MSG 'AP01'


AudioCDPanel::AudioCDPanel(BRect frame):BView(frame, "AudioCDPanel", B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW) {
	cdtextCB=new BCheckBox(BRect(frame.left+4, frame.top, frame.right-4, frame.top+16), "acdp_writeCDText",
									_T("Write CD text"), // "TWRITECDTEXT"
									new BMessage(CDTEXTBOX_MSG));
	copyprotectionCB=new BCheckBox(BRect(frame.left+4, frame.top, frame.right-4, frame.top+16), "acdp_copyprotection",
									_T("Add SCMS-Copyright"), // "TCOPYPROTECTION"
									NULL);
	CDTextAlbum=new BTextControl(BRect(frame.left+4, frame.top, frame.right-4, frame.top+16), "acdp_cdtextalbum",
									_T("CD text album"), // "TCDTEXTALBUMLABEL"
									"", NULL);
	AddChild(copyprotectionCB);
	AddChild(cdtextCB);
	AddChild(CDTextAlbum);
	SetViewColor((rgb_color){216,216,216,255});
}

AudioCDPanel::~AudioCDPanel() {
	delete cdtextCB;
	delete copyprotectionCB;
	delete CDTextAlbum;
}

void AudioCDPanel::AllAttached() {
	copyprotectionCB->MoveTo(5,7);
	copyprotectionCB->ResizeTo(22+StringWidth(copyprotectionCB->Label()), 16);
	cdtextCB->MoveTo(5,28);
	cdtextCB->ResizeTo(22+StringWidth(cdtextCB->Label()), 16);
	
	CDTextAlbum->MoveTo(5, 53);
	CDTextAlbum->ResizeTo(200, 20);
	// initializes the textcontrol with the previously loaded user-settings
	CDTextAlbum->SetDivider(StringWidth(_T("Volume name"))+28); // "TVOLUMENAME"
	CDTextAlbum->SetEnabled(cdtextCB->Value()==B_CONTROL_ON);
	cdtextCB->SetTarget(this);
}

void AudioCDPanel::MessageReceived(BMessage *msg) {
	switch(msg->what) {
		case CDTEXTBOX_MSG:
			CDTextAlbum->SetEnabled(cdtextCB->Value()==B_CONTROL_ON);
			break;
			
		default:
			BView::MessageReceived(msg);
			break;
	}
}

bool AudioCDPanel::writeCDText() {
	return (cdtextCB->Value()==B_CONTROL_ON);
}

void AudioCDPanel::SetCDText(bool flag) {
	if (flag)  	cdtextCB->SetValue(B_CONTROL_ON); else
			cdtextCB->SetValue(B_CONTROL_OFF);
}

bool AudioCDPanel::copyProtection() {
	return (copyprotectionCB->Value()==B_CONTROL_ON);
}

void AudioCDPanel::SetProtection(bool flag) {
	if (flag)  	copyprotectionCB->SetValue(B_CONTROL_ON); else
			copyprotectionCB->SetValue(B_CONTROL_OFF);
}

void AudioCDPanel::SetCDAlbum(const char *name) {
	CDTextAlbum->SetText(name);
}

const char *AudioCDPanel::GetCDAlbum() {
	return CDTextAlbum->Text();
}

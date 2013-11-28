#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str)	gMSHLangMgr->_T(str).String()
#define _TPS(str)	gMSHLangMgr->_T(str)
#else
#include <locale/Locale.h>
#endif

#include "SoundConfigView.h"

SoundConfigView::SoundConfigView(BRect frame) :
	ConfigView(frame)
{
	// CHECKBOXES
	writtensoundCB=new BCheckBox(BRect(15,5,230,20), "scv_writtensoundCB",
		_T("Beep when finished recording disc"), // "TBEEPWHENFINISHEDDISC"
		NULL);
	writtensoundCB->ResizeToPreferred();
	AddChild(writtensoundCB);
	imagesoundCB=new BCheckBox(BRect(15,writtensoundCB->Frame().bottom + 5,230,40), "scv_imagesoundCB",
		_T("Beep when finished creating iImage"), // "TBEEPWHENFINISHEDIMAGE"
		NULL);
	imagesoundCB->ResizeToPreferred();
	AddChild(imagesoundCB);
	errorsoundCB=new BCheckBox(BRect(15,imagesoundCB->Frame().bottom + 5,230,60), "scv_errorsoundCB",
		_T("Beep when an error occurs"), // "TBEEPWHENERROROCCURS"
		NULL);
	errorsoundCB->ResizeToPreferred();
	AddChild(errorsoundCB);
}

SoundConfigView::~SoundConfigView() {
}

bool	SoundConfigView::HasFinishedWritingSound() {
	return writtensoundCB->Value()==B_CONTROL_ON;
}

void	SoundConfigView::SetFinishedWritingSound(bool state) {
	writtensoundCB->SetValue(state ? B_CONTROL_ON : B_CONTROL_OFF);
}

bool	SoundConfigView::HasFinishedImageSound() {
	return imagesoundCB->Value()==B_CONTROL_ON;
}

void	SoundConfigView::SetFinishedImageSound(bool state) {
	imagesoundCB->SetValue(state ? B_CONTROL_ON : B_CONTROL_OFF);
}

bool	SoundConfigView::HasErrorSound() {
	return errorsoundCB->Value()==B_CONTROL_ON;
}

void	SoundConfigView::SetErrorSound(bool state) {
	errorsoundCB->SetValue(state ? B_CONTROL_ON : B_CONTROL_OFF);
}

// ... (see header file)
const char	*SoundConfigView::GetFinishedWritingSoundFile() {
	return "";
}

void	SoundConfigView::SetFinishedWritingSoundFile(const char *) {
}

const char	*SoundConfigView::GetFinishedImageSoundFile() {
	return "";
}

void	SoundConfigView::SetFinishedImageSoundFile(const char *) {
}


// PRIVATE

void	SoundConfigView::MessageReceived(BMessage *msg) {
	switch(msg->what) {
		default: {
			BView::MessageReceived(msg);
			break;
		}
	}
}

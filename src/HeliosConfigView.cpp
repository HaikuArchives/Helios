#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str) gMSHLangMgr->_T(str).String()
#define _TPS(str) gMSHLangMgr->_T(str)
#else
#include <locale/Locale.h>
#endif

#include "Application1.h"
#include "HeliosConfigView.h"

#define SPEEDSLIDER_CHANGED 'HCV0'
#define FIFOSLIDER_CHANGED 'HCV1'
#define DAO_MSG 'HCV2'

HeliosConfigView::HeliosConfigView(BRect frame) : ConfigView(frame)
{
	// CHECKBOXES
	simulationCB =
		new BCheckBox(BRect(15, 5, 230, 20), "hcv_simulationCB", _T("Simulation"), // "TSIMULATION"
					  NULL);
	AddChild(simulationCB);
	fixatingCB = new BCheckBox(BRect(15, 25, 230, 40), "hcv_fixatingCB", _T("Fixate"), // "TFIXATE"
							   NULL);
	AddChild(fixatingCB);
	burnproofCB =
		new BCheckBox(BRect(15, 45, 230, 60), "hcv_burnproofCB", _T("BurnProof"), // "TBURNPROOF"
					  NULL);
	AddChild(burnproofCB);

	forcedCB = new BCheckBox(BRect(235, 5, (frame.right - 15), 20), "hcv_forcedCB",
							 _T("Force RW-erasing"), // "TFORCEOPTION"
							 NULL);
	AddChild(forcedCB);
	DAOCB = new BCheckBox(BRect(235, 25, (frame.right - 15), 40), "hcv_DAOCB",
						  _T("Disc-at-Once"), // "TDAOMODE"
						  new BMessage(DAO_MSG));
	AddChild(DAOCB);
	overburningCB = new BCheckBox(BRect(235, 45, (frame.right - 15), 60), "hcv_overburningCB",
								  _T("allow CDs > 80 min"), // "TALLOWOVERBURNING"
								  NULL);
	AddChild(overburningCB);

	//	generalFlagsTC=new BTextControl(BRect(15,115,(frame.right - 15),130), "hcv_generalTC",
	//_T("General Flags"),
	generalFlagsTC = new BTextControl(BRect(15, 65, (frame.right - 15), 80), "hcv_generalTC",
									  _T("General Flags"), "", NULL);
	generalFlagsTC->SetDivider(StringWidth(_T("General Flags")) + 5);
	AddChild(generalFlagsTC);

	// SLIDERS
	BRect viewRect = Bounds();
	//	speedSL=new BSlider(BRect(15, 164, 325, 198), "hcv_speedSL", NULL, new
	//BMessage(SPEEDSLIDER_CHANGED), 0, 24);
	speedSL = new BSlider(BRect(15, 164, viewRect.right - 15, 198), "hcv_speedSL", NULL,
						  new BMessage(SPEEDSLIDER_CHANGED), 0, 24);
	speedSL->SetHashMarks(B_HASH_MARKS_BOTTOM);
	speedSL->SetHashMarkCount(25);
	speedSL->SetKeyIncrementValue(1);
	speedSL->SetLimitLabels("1x", "48x");
	AddChild(speedSL);
	fifoSL = new BSlider(BRect(15, 216, viewRect.right - 15, 250), "hcv_fifoSL", NULL,
						 new BMessage(FIFOSLIDER_CHANGED), 1, 32);
	fifoSL->SetHashMarks(B_HASH_MARKS_BOTTOM);
	fifoSL->SetHashMarkCount(32);
	fifoSL->SetKeyIncrementValue(1);
	fifoSL->SetLimitLabels("1MB", "32MB");
	AddChild(fifoSL);

	// STRINGVIEWS
	speedSV = new BStringView(BRect(15, 148, 300, 163), "speedSV", "");
	AddChild(speedSV);
	fifoSV = new BStringView(BRect(15, 200, 300, 215), "fifoSV", "");
	AddChild(fifoSV);
}

HeliosConfigView::~HeliosConfigView()
{
}

bool HeliosConfigView::IsSimulation()
{
	return simulationCB->Value() == B_CONTROL_ON;
}

void HeliosConfigView::SetSimulation(bool state)
{
	simulationCB->SetValue(state ? B_CONTROL_ON : B_CONTROL_OFF);
}

bool HeliosConfigView::IsFixating()
{
	return fixatingCB->Value() == B_CONTROL_ON;
}

void HeliosConfigView::SetFixating(bool state)
{
	fixatingCB->SetValue(state ? B_CONTROL_ON : B_CONTROL_OFF);
}

bool HeliosConfigView::IsBurnproof()
{
	return burnproofCB->Value() == B_CONTROL_ON;
}

void HeliosConfigView::SetBurnproof(bool state)
{
	burnproofCB->SetValue(state ? B_CONTROL_ON : B_CONTROL_OFF);
}

bool HeliosConfigView::IsDAO()
{
	return DAOCB->Value() == B_CONTROL_ON;
}

void HeliosConfigView::SetDAO(bool state)
{
	DAOCB->SetValue(state ? B_CONTROL_ON : B_CONTROL_OFF);
}

bool HeliosConfigView::IsOverburning()
{
	return overburningCB->Value() == B_CONTROL_ON;
}

void HeliosConfigView::SetOverburning(bool state)
{
	overburningCB->SetValue(state ? B_CONTROL_ON : B_CONTROL_OFF);
}

int32 HeliosConfigView::GetWriterSpeed()
{
	if (speedSL->Value() == 0)
		return 1;
	else
		return speedSL->Value() * 2;
}

void HeliosConfigView::SetWriterSpeed(int32 speed)
{
	if (speed == 1)
		speedSL->SetValue(0);
	else
		speedSL->SetValue(speed / 2);
	string = _T("Recording speed"); // "TRECORDERSPEED"
	string << " (" << speed << "x)";
	speedSV->SetText(string.String());
}

int8 HeliosConfigView::GetFIFOSize()
{
	return fifoSL->Value();
}

void HeliosConfigView::SetFIFOSize(int8 size)
{
	fifoSL->SetValue(size);
	string = _T("FIFO buffer size"); // "TBUFFERSIZE"
	string << " " << size << " MB";
	fifoSV->SetText(string.String());
}

bool HeliosConfigView::IsForcedErasing()
{
	return forcedCB->Value() == B_CONTROL_ON;
}

void HeliosConfigView::SetForcedErasing(bool state)
{
	forcedCB->SetValue(state ? B_CONTROL_ON : B_CONTROL_OFF);
}

BString HeliosConfigView::GetGeneralFlags()
{
	return generalFlagsTC->Text();
}

void HeliosConfigView::SetGeneralFlags(BString general)
{
	generalFlagsTC->SetText(general.String());
}

// PRIVATE

void HeliosConfigView::AllAttached()
{
	speedSL->SetModificationMessage(new BMessage(SPEEDSLIDER_CHANGED));
	speedSL->SetTarget(this);
	fifoSL->SetModificationMessage(new BMessage(FIFOSLIDER_CHANGED));
	fifoSL->SetTarget(this);
	DAOCB->SetTarget(this, Looper());
	ConfigView::AllAttached();
}

void HeliosConfigView::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
	case FIFOSLIDER_CHANGED: {
		SetFIFOSize(GetFIFOSize());
		break;
	}
	case SPEEDSLIDER_CHANGED: {
		SetWriterSpeed(GetWriterSpeed());
		break;
	}
	case DAO_MSG: {
		Application1* app = (Application1*)be_app;

		if (app->window1->Lock()) {
			if (app->window1->view1->datapanel->IsMultisession()) {
				app->window1->view1->datapanel->SetMultisession(false);
				if (app->window1->Lock()) {
					app->window1->view1->statusBAR->SetText(
						_T("Multisession disabled."), // "L:Multisession disabled."
						4000000);
					app->window1->Unlock();
				}
			}
			app->window1->Unlock();
		}
		break;
	}
	default: {
		BView::MessageReceived(msg);
		break;
	}
	}
}

void HeliosConfigView::GetSettings(BMessage* archive)
{
	// CHECKBOXES
	archive->AddBool("HeliosConfigView::hcv_simulationCB",
					 ((BCheckBox*)FindView("hcv_simulationCB"))->Value() == B_CONTROL_ON);
	archive->AddBool("HeliosConfigView::hcv_fixatingCB",
					 ((BCheckBox*)FindView("hcv_fixatingCB"))->Value() == B_CONTROL_ON);
	archive->AddBool("HeliosConfigView::hcv_burnproofCB",
					 ((BCheckBox*)FindView("hcv_burnproofCB"))->Value() == B_CONTROL_ON);
	archive->AddBool("HeliosConfigView::hcv_forcedCB",
					 ((BCheckBox*)FindView("hcv_forcedCB"))->Value() == B_CONTROL_ON);
	archive->AddBool("HeliosConfigView::hcv_DAOCB",
					 ((BCheckBox*)FindView("hcv_DAOCB"))->Value() == B_CONTROL_ON);
	archive->AddBool("HeliosConfigView::hcv_overburningCB",
					 ((BCheckBox*)FindView("hcv_overburningCB"))->Value() == B_CONTROL_ON);

	// SLIDERS
	archive->AddInt32("HeliosConfigView::hcv_speedSL",
					  ((BSlider*)FindView("hcv_speedSL"))->Value());
	archive->AddInt32("HeliosConfigView::hcv_fifoSL", ((BSlider*)FindView("hcv_fifoSL"))->Value());
	archive->AddString("HeliosConfigView::hcv_generalTC",
					   ((BTextControl*)FindView("hcv_generalTC"))->Text());
}

void HeliosConfigView::SetSettings(BMessage* archive)
{
	bool boolvar;
	int32 int32var;
	BString string;

	// CHECKBOXES
	if (archive->FindBool("HeliosConfigView::hcv_simulationCB", &boolvar) == B_OK)
		((BCheckBox*)FindView("hcv_simulationCB"))
			->SetValue(boolvar ? B_CONTROL_ON : B_CONTROL_OFF);
	else
		((BCheckBox*)FindView("hcv_simulationCB"))->SetValue(B_CONTROL_OFF);
	if (archive->FindBool("HeliosConfigView::hcv_fixatingCB", &boolvar) == B_OK)
		((BCheckBox*)FindView("hcv_fixatingCB"))->SetValue(boolvar ? B_CONTROL_ON : B_CONTROL_OFF);
	else
		((BCheckBox*)FindView("hcv_fixatingCB"))->SetValue(B_CONTROL_ON);
	if (archive->FindBool("HeliosConfigView::hcv_burnproofCB", &boolvar) == B_OK)
		((BCheckBox*)FindView("hcv_burnproofCB"))->SetValue(boolvar ? B_CONTROL_ON : B_CONTROL_OFF);
	else
		((BCheckBox*)FindView("hcv_burnproofCB"))->SetValue(B_CONTROL_OFF);
	if (archive->FindBool("HeliosConfigView::hcv_forcedCB", &boolvar) == B_OK)
		((BCheckBox*)FindView("hcv_forcedCB"))->SetValue(boolvar ? B_CONTROL_ON : B_CONTROL_OFF);
	else
		((BCheckBox*)FindView("hcv_forcedCB"))->SetValue(B_CONTROL_OFF);
	if (archive->FindBool("HeliosConfigView::hcv_DAOCB", &boolvar) == B_OK)
		((BCheckBox*)FindView("hcv_DAOCB"))->SetValue(boolvar ? B_CONTROL_ON : B_CONTROL_OFF);
	else
		((BCheckBox*)FindView("hcv_DAOCB"))->SetValue(B_CONTROL_ON);
	if (archive->FindBool("HeliosConfigView::hcv_overburningCB", &boolvar) == B_OK)
		((BCheckBox*)FindView("hcv_overburningCB"))
			->SetValue(boolvar ? B_CONTROL_ON : B_CONTROL_OFF);
	else
		((BCheckBox*)FindView("hcv_overburningCB"))->SetValue(B_CONTROL_OFF);

	if (archive->FindInt32("HeliosConfigView::hcv_speedSL", &int32var) == B_OK)
		((BSlider*)FindView("hcv_speedSL"))->SetValue(int32var);
	else
		((BSlider*)FindView("hcv_speedSL"))->SetValue(10);
	if (archive->FindInt32("HeliosConfigView::hcv_fifoSL", &int32var) == B_OK)
		((BSlider*)FindView("hcv_fifoSL"))->SetValue(int32var);
	else
		((BSlider*)FindView("hcv_fifoSL"))->SetValue(3);

	if (archive->FindString("HeliosConfigView::hcv_generalTC", &string) == B_OK) {
		((BTextControl*)FindView("hcv_generalTC"))->SetText(string.String());
	} else {
		((BTextControl*)FindView("hcv_generalTC"))->SetText("");
	}

	SetFIFOSize(GetFIFOSize());
	SetWriterSpeed(GetWriterSpeed());
}

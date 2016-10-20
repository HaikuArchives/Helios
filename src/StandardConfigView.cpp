#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str) gMSHLangMgr->_T(str).String()
#define _TPS(str) gMSHLangMgr->_T(str)
#else
#include <locale/Locale.h>
#endif

#include "StandardConfigView.h"

StandardConfigView::StandardConfigView(BRect frame) : ConfigView(frame)
{
	BFont* TCfont = new BFont(be_fixed_font);
	TCfont->SetSize(10.0);
	// Note: we set the font on the view purely to allow for string width calculations.
	SetFont(TCfont);

	const BString cdPublisherLabel = _T("CD Publisher:");
	const BString cdPreparerLabel = _T("CD Preparer:");
	const BString cdApplicationLabel = _T("CD Application:");
	const BString cdFilesystemLabel = _T("Filesystem");
	const BString cdVolumeNameLabel = _T("Volume name");
	const BString cdTypeLabel = _T("CD type");

	// Calculate widths of strings, and then use the width of the widest string to set
	// the left-hand side position of all text fields.
	const float publisherWidth = StringWidth(cdPublisherLabel.String());
	const float preparerWidth = StringWidth(cdPreparerLabel.String());
	const float applicationWidth = StringWidth(cdApplicationLabel.String());
	const float filesystemWidth = StringWidth(cdFilesystemLabel.String());
	const float volumeNameWidth = StringWidth(cdVolumeNameLabel.String());
	const float cdTypeWidth = StringWidth(cdTypeLabel.String());

	float greatestWidth = (publisherWidth > preparerWidth) ? publisherWidth : preparerWidth;
	greatestWidth = (greatestWidth > applicationWidth) ? greatestWidth : applicationWidth;
	greatestWidth = (greatestWidth > filesystemWidth) ? greatestWidth : filesystemWidth;
	greatestWidth = (greatestWidth > volumeNameWidth) ? greatestWidth : volumeNameWidth;
	greatestWidth = (greatestWidth > cdTypeWidth) ? greatestWidth : cdTypeWidth;

	// TEXTCONTROLS
	publisherTC = new BTextControl(BRect(15, 8, frame.Width() - 15, 33), "scv_publisherTC",
								   cdPublisherLabel.String(), "", NULL); // "TPUBLISHER"
	publisherTC->SetDivider(greatestWidth);								 // "TPUBLISHER"
	publisherTC->TextView()->SetMaxBytes(128);
	publisherTC->TextView()->SetFontAndColor(TCfont);
	AddChild(publisherTC);

	preparerTC = new BTextControl(BRect(15, 38, frame.Width() - 15, 63), "scv_preparerTC",
								  cdPreparerLabel.String(), "", NULL); // "TPREPARER"
	preparerTC->SetDivider(greatestWidth);							   // "TPREPARER"
	preparerTC->TextView()->SetMaxBytes(128);
	preparerTC->TextView()->SetFontAndColor(TCfont);
	AddChild(preparerTC);

	applicationTC = new BTextControl(BRect(15, 68, frame.Width() - 15, 93), "scv_applicationTC",
									 cdApplicationLabel.String(), "", NULL); // "TAPPLICATION"
	applicationTC->SetDivider(greatestWidth);								 // "TAPPLICATION"
	applicationTC->TextView()->SetMaxBytes(128);
	applicationTC->TextView()->SetFontAndColor(TCfont);
	AddChild(applicationTC);

	//	projectpathTC=new
	//BrowseField(BRect(15,98,340,123),"scv_projectpathTC");//,_T("TPROJECTPATH"), "",NULL);
	//	projectpathTC->GetTextControl()->SetDivider(100);
	//	projectpathTC->GetButton()->SetLabel(_T("TBROWSE"));
	//	projectpathTC->Init(_T("C_SELECT"), NULL, B_FILE_NODE|B_DIRECTORY_NODE);
	//	projectpathTC->GetTextControl()->SetLabel(_T("TPROJECTPATH"));
	//	projectpathTC->GetTextControl()->TextView()->SetMaxBytes(8192);
	// AddChild(projectpathTC);

	//	temppathTC=new BrowseField(BRect(15,128,340,153),"scv_temppathTC");//,_T("TTEMPPATH"),
	//"",NULL);
	//	temppathTC->GetTextControl()->SetDivider(100);
	//	temppathTC->GetButton()->SetLabel(_T("TBROWSE"));
	//	temppathTC->Init(_T("C_SELECT"), NULL, B_FILE_NODE|B_DIRECTORY_NODE);
	//	temppathTC->GetTextControl()->SetLabel(_T("TTEMPPATH"));
	//	temppathTC->GetTextControl()->TextView()->SetMaxBytes(8192);
	// AddChild(temppathTC);

	// FILESYSTEM MENU
	filesystemPUM = new BPopUpMenu("");
	filesystemMF =
		new BMenuField(BRect(15, 98, 230, 123), "scv_filesystemMF", cdFilesystemLabel.String(),
					   filesystemPUM, false); // "TFILESYSTEM"
	filesystemMF->SetDivider(greatestWidth);

	filesystemPUM->AddItem(new BMenuItem(_T("ISO9660"), NULL));		   // "MI:ISO9660"
	filesystemPUM->AddItem(new BMenuItem(_T("ISO9660 long"), NULL));   // "MI:ISO9660 long"
	filesystemPUM->AddItem(new BMenuItem(_T("BeOS bfs"), NULL));	   // "MI:BeOS bfs"
	filesystemPUM->AddItem(new BMenuItem(_T("MacOS hfs"), NULL));	  // "MI:MacOS hfs"
	filesystemPUM->AddItem(new BMenuItem(_T("Unix RockRidge"), NULL)); // "MI:Unix RockRidge"
	filesystemPUM->AddItem(new BMenuItem(_T("Windows Joliet"), NULL)); // "MI:Windows Joliet"
	filesystemPUM->AddItem(new BMenuItem(_T("UDF"), NULL));			   // "MI:UDF"
	filesystemPUM->AddSeparatorItem();
	filesystemPUM->AddItem(new BMenuItem(_T("Custom"), NULL)); // "MI:Custom"
	AddChild(filesystemMF);

	// VOLUMENAME
	volumenameTC = new BTextControl(BRect(15, 128, frame.Width() - 15, 153), "scv_volumenameTC",
									cdVolumeNameLabel.String(), "", NULL); // "TVOLUMENAME"
	volumenameTC->SetDivider(greatestWidth);
	volumenameTC->TextView()->SetMaxBytes(8192);
	volumenameTC->TextView()->SetFontAndColor(TCfont);
	AddChild(volumenameTC);

	CDTypeM = new CDTypeMenu(BRect(15, 158, 200, 183), NULL);
	CDTypeM->SetLabel(cdTypeLabel.String()); // "TDEFAULTDISCMODE"
	CDTypeM->SetDivider(greatestWidth);
	AddChild(CDTypeM);
}

StandardConfigView::~StandardConfigView()
{
}

int32 StandardConfigView::GetFilesystem()
{
	return filesystemPUM->IndexOf(filesystemPUM->FindMarked());
}

void StandardConfigView::SetFilesystem(int32 item)
{
	if (filesystemPUM->ItemAt(item) != NULL) (filesystemPUM->ItemAt(item))->SetMarked(true);
}

const char* StandardConfigView::GetVolumeName()
{
	return volumenameTC->Text();
}

void StandardConfigView::SetVolumeName(const char* name)
{
	volumenameTC->SetText(name);
}

int8 StandardConfigView::GetCDType()
{
	return CDTypeM->GetMenu()->IndexOf(CDTypeM->GetMenu()->FindMarked());
}

void StandardConfigView::SetCDType(int8 type)
{
	(CDTypeM->GetMenu()->ItemAt(type))->SetMarked(true);
}

const char* StandardConfigView::GetPublisher()
{
	return publisherTC->Text();
}

void StandardConfigView::SetPublisher(const char* str)
{
	publisherTC->SetText(str);
}

const char* StandardConfigView::GetPreparer()
{
	return preparerTC->Text();
}

void StandardConfigView::SetPreparer(const char* str)
{
	preparerTC->SetText(str);
}

const char* StandardConfigView::GetApplication()
{
	return applicationTC->Text();
}

void StandardConfigView::SetApplication(const char* str)
{
	applicationTC->SetText(str);
}

// const char *StandardConfigView::GetTemporaryPath() {
//	return temppathTC->GetTextControl()->Text();
//}
//
// void StandardConfigView::SetTemporaryPath(const char *path) {
//	temppathTC->GetTextControl()->SetText(path);
//}
//
// const char *StandardConfigView::GetProjectPath() {
//	return projectpathTC->GetTextControl()->Text();
//}
//
// void StandardConfigView::SetProjectPath(const char *path) {
//	projectpathTC->GetTextControl()->SetText(path);
//}

// PRIVATE

void StandardConfigView::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
	default: {
		BView::MessageReceived(msg);
		break;
	}
	}
}

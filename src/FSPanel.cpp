#ifndef _ZETA_USING_DEPRECATED_API_
#define _ZETA_USING_DEPRECATED_API_
#endif
	// To allow PostMessage() to work. Grrr...

#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str)	gMSHLangMgr->_T(str).String()
#define _TPS(str)	gMSHLangMgr->_T(str)
#else
#include <locale/Locale.h>
#endif

#include "FSPanel.h"

#define ISO9660_CB_MSG	'FSP0'
#define JOLIET_CB_MSG	'FSP1'
#define ROCKRIDGE_CB_MSG	'FSP2'
#define HFS_CB_MSG	'FSP3'
#define UDF_CB_MSG	'FSP4'
#define BFS_CB_MSG	'FSP5'
#define MAPPING_CB_MSG	'FSP6'

FSPanel::FSPanel(BRect frame)
	: ConfigView(frame) {
	
	SetViewColor(ui_color( B_PANEL_BACKGROUND_COLOR ));
	
	const float oneThirdAcross = frame.Width()*1/3;
	const float twoThirdsAcross = frame.Width()*2/3;
	
	// View
	ISO9660_V=new BView(BRect(15,5,119,100), "FSP_ISO9660_V", B_FOLLOW_LEFT | B_FOLLOW_TOP, 0);
	ISO9660_V->SetViewColor(ui_color( B_PANEL_BACKGROUND_COLOR ));
	// Controls
	ISO9660_CB=new BCheckBox(BRect(0,0,103,20), "fsp_iso9660_cb", _T("ISO9660"), // "CB:ISO9660"
					new BMessage(ISO9660_CB_MSG));
	ISO9660_CB->SetValue(B_CONTROL_ON);
	ISO9660_V->AddChild(ISO9660_CB);
	F83_RB=new BRadioButton(BRect(15,22,103, 42), "fsp_f83_rb", _T("8.3"), // "RB:8.3"
					NULL);
	F83_RB->SetValue(B_CONTROL_ON);
	ISO9660_V->AddChild(F83_RB);
	F31_RB=new BRadioButton(BRect(15,44,103, 64), "fsp_f31_rb", _T("31"), // "RB:31"
					NULL);
	ISO9660_V->AddChild(F31_RB);
	F37_RB=new BRadioButton(BRect(15,66,103, 86), "fsp_f37_rb", _T("37"), // "RB:37"
					NULL);
	ISO9660_V->AddChild(F37_RB);
	
	AddChild(ISO9660_V);


	// View
	Joliet_V=new BView(BRect(oneThirdAcross,5,229,100), "FSP_Joliet_V", B_FOLLOW_LEFT | B_FOLLOW_TOP, 0);
	Joliet_V->SetViewColor(ui_color( B_PANEL_BACKGROUND_COLOR ));
	// Controls
	Joliet_CB=new BCheckBox(BRect(0,0,112,20), "fsp_Joliet_CB", _T("Joliet"), // "CB:Joliet"
					new BMessage(JOLIET_CB_MSG));
	Joliet_V->AddChild(Joliet_CB);
	JolietDef_RB=new BRadioButton(BRect(15,22,112, 42), "fsp_JolietDef_RB", _T("Default"), // "RB:Default"
					NULL);
	JolietDef_RB->SetValue(B_CONTROL_ON);
	Joliet_V->AddChild(JolietDef_RB);
	JolietUni_RB=new BRadioButton(BRect(15,44,112, 64), "fsp_JolietUni_RB", _T("Unicode"), // "RB:Unicode"
					NULL);
	Joliet_V->AddChild(JolietUni_RB);
	
	AddChild(Joliet_V);


	// View
	RockRidge_V=new BView(BRect(twoThirdsAcross,5,frame.Width()-15,100), "FSP_RockRidge_V", B_FOLLOW_LEFT | B_FOLLOW_TOP, 0);
	RockRidge_V->SetViewColor(ui_color( B_PANEL_BACKGROUND_COLOR ));
	// Controls
	RockRidge_CB=new BCheckBox(BRect(0,0,112,20), "fsp_RockRidge_CB", _T("RockRidge"), // "CB:RockRidge"
					new BMessage(ROCKRIDGE_CB_MSG));
	RockRidge_V->AddChild(RockRidge_CB);
	RRDef_RB=new BRadioButton(BRect(15,22,112, 42), "fsp_RRDef_RB", _T("Default"), // "RB:Default"
					NULL);
	RRDef_RB->SetValue(B_CONTROL_ON);
	RockRidge_V->AddChild(RRDef_RB);
	RRRat_RB=new BRadioButton(BRect(15,44,112, 64), "fsp_RRRat_RB", _T("Rationalized"), // "RB:Rationalized"
					NULL);
	RockRidge_V->AddChild(RRRat_RB);
	
	AddChild(RockRidge_V);
	
	
	// View
	HFS_V=new BView(BRect(15,105,frame.Width()-15,180), "FSP_HFS_V", B_FOLLOW_LEFT | B_FOLLOW_TOP, 0);
	HFS_V->SetViewColor( ui_color( B_PANEL_BACKGROUND_COLOR ) );
	// Controls
	HFS_CB=new BCheckBox(BRect(0,0,100,20), "fsp_HFS_CB", _T("HFS/ISO Hybrid"), // "CB:HFS/ISO Hybrid"
					new BMessage(HFS_CB_MSG));
	HFS_CB->ResizeToPreferred();
	HFS_V->AddChild(HFS_CB);
	mappingCB=new BCheckBox(BRect(15,22,170,42), "fsp_mappingCB", _T("Use mapping file:"), // "CB:Use Mapping File:"
					new BMessage(MAPPING_CB_MSG));
	HFS_V->AddChild(mappingCB);
	
	BRect HFS_VRect = HFS_V->Bounds();
	mappingBF=new BrowseField(BRect(30, 44, HFS_VRect.Width(), 69), "fsp_mappingBF", B_FOLLOW_LEFT|B_FOLLOW_TOP, 0);
	mappingBF->GetTextControl()->SetDivider(-1);
	mappingBF->GetButton()->SetLabel(_T("Browse")); // "TBROWSE"
	mappingBF->GetTextControl()->SetLabel("");
	mappingBF->GetTextControl()->TextView()->SetMaxBytes(8192);
	mappingBF->Init(_T("Select mapping file")); // "L:Select Mapping File"
	HFS_V->AddChild(mappingBF);
	
	AddChild(HFS_V);
	
	
	
	// View
	UDF_V=new BView(BRect(15,190,136,265), "FSP_UDF_V", B_FOLLOW_LEFT | B_FOLLOW_TOP, 0);
	UDF_V->SetViewColor(ui_color( B_PANEL_BACKGROUND_COLOR ));
	// Controls
	UDF_CB=new BCheckBox(BRect(0,0,90,20), "fsp_UDF_CB", _T("UDF"), // "CB:UDF"
					new BMessage(UDF_CB_MSG));
	UDF_V->AddChild(UDF_CB);
	UDFDef_RB=new BRadioButton(BRect(15,22,120, 42), "fsp_UDFDef_RB", _T("Default"), // "RB:Default"
					NULL);
	UDFDef_RB->SetValue(B_CONTROL_ON);
	UDF_V->AddChild(UDFDef_RB);
	UDFDVD_RB=new BRadioButton(BRect(15,44,120, 64), "fsp_UDFDVD_RB", _T("DVD Video"), // "RB:DVD Video"
					NULL);
	UDF_V->AddChild(UDFDVD_RB);
	
	AddChild(UDF_V);


	// View
	BFS_V=new BView(BRect(twoThirdsAcross,190,frame.Width()-15,265), "FSP_BFS_V", B_FOLLOW_LEFT | B_FOLLOW_TOP, 0);
	BFS_V->SetViewColor(ui_color( B_PANEL_BACKGROUND_COLOR ));
	// Controls
	BFS_CB=new BCheckBox(BRect(0,0,100,20), "fsp_BFS_CB", _T("BFS"), // "CB:BFS"
					new BMessage(BFS_CB_MSG));
	BFS_V->AddChild(BFS_CB);
	
	AddChild(BFS_V);
	
}


FSPanel::~FSPanel() {
}


void FSPanel::CheckCB() {
	F83_RB->SetEnabled(GetISO9660());
	F31_RB->SetEnabled(GetISO9660());
	F37_RB->SetEnabled(GetISO9660());
	
	Joliet_CB->SetEnabled(GetISO9660());
	RockRidge_CB->SetEnabled(GetISO9660());
	HFS_CB->SetEnabled(GetISO9660());
	mappingCB->SetEnabled((HFS_CB->Value()==B_CONTROL_ON) && (HFS_CB->IsEnabled()));

	Looper()->PostMessage(JOLIET_CB_MSG, this);
	Looper()->PostMessage(ROCKRIDGE_CB_MSG, this);
	Looper()->PostMessage(MAPPING_CB_MSG, this);

	
	if (UDF_CB->IsEnabled()) {
		UDFDef_RB->SetEnabled(GetUDF());
		UDFDVD_RB->SetEnabled(GetUDF());
	} else {
		UDFDef_RB->SetEnabled(false);
		UDFDVD_RB->SetEnabled(false);
	}

	if (ISO9660_CB->Value()==B_CONTROL_OFF &&
		UDF_CB->Value()==B_CONTROL_OFF &&
		BFS_CB->Value()==B_CONTROL_OFF) {
		
		BFS_CB->SetValue(B_CONTROL_ON);
		CheckCB();
	}
}


void FSPanel::MessageReceived(BMessage *msg) {
	switch(msg->what) {
		case ISO9660_CB_MSG: {
			BFS_CB->SetValue(B_CONTROL_OFF);
			CheckCB();
			break;
		}
		case JOLIET_CB_MSG: {
			if (Joliet_CB->IsEnabled()) {
				JolietDef_RB->SetEnabled(GetJoliet());
				JolietUni_RB->SetEnabled(GetJoliet());
			} else {
				JolietDef_RB->SetEnabled(false);
				JolietUni_RB->SetEnabled(false);
			}
			break;
		}
		
		case ROCKRIDGE_CB_MSG: {
			if (RockRidge_CB->IsEnabled()) {
				RRDef_RB->SetEnabled(GetRockRidge());
				RRRat_RB->SetEnabled(GetRockRidge());
			} else {
				RRDef_RB->SetEnabled(false);
				RRRat_RB->SetEnabled(false);
			}
			break;
		}

		case HFS_CB_MSG: {
			CheckCB();
			break;
		}

		case UDF_CB_MSG: {
			BFS_CB->SetValue(B_CONTROL_OFF);
			CheckCB();
			break;
		}
		
		case BFS_CB_MSG: {
			ISO9660_CB->SetValue(B_CONTROL_OFF);
			UDF_CB->SetValue(B_CONTROL_OFF);
			CheckCB();
			break;
		}
		
		case MAPPING_CB_MSG: {
			if (mappingCB->IsEnabled()) {
				if (mappingCB->Value()==B_CONTROL_ON) {
					mappingBF->GetButton()->SetEnabled(true);
					mappingBF->GetTextControl()->SetEnabled(true);
				} else {
					mappingBF->GetButton()->SetEnabled(false);
					mappingBF->GetTextControl()->SetEnabled(false);
				}
			} else {
				mappingBF->GetButton()->SetEnabled(false);
				mappingBF->GetTextControl()->SetEnabled(false);
			}
			break;
		}

		default: ConfigView::MessageReceived(msg);break;
	}
}


void FSPanel::AllAttached() {
	ISO9660_CB->SetTarget(this, Looper());
	Joliet_CB->SetTarget(this, Looper());
	RockRidge_CB->SetTarget(this, Looper());
	HFS_CB->SetTarget(this, Looper());
	UDF_CB->SetTarget(this, Looper());
	BFS_CB->SetTarget(this, Looper());
	mappingCB->SetTarget(this, Looper());
	
	Looper()->PostMessage(ISO9660_CB_MSG, this);
	Looper()->PostMessage(UDF_CB_MSG, this);
}


BList	*FSPanel::GetArgumentList() {
	CheckCB();
	BList	*list=new BList();
	if (ISO9660_CB->Value()==B_CONTROL_ON) {
		if (F31_RB->Value()==B_CONTROL_ON)
			list->AddItem(new BString("-l"));
		if (F37_RB->Value()==B_CONTROL_ON)
			list->AddItem(new BString("-max-iso9660-filenames"));
		
		if (Joliet_CB->Value()==B_CONTROL_ON) {
			list->AddItem(new BString("-J"));
			if (JolietUni_RB->Value()==B_CONTROL_ON)
				list->AddItem(new BString("-joliet-long"));
		}
		if (RockRidge_CB->Value()==B_CONTROL_ON) {
			if (RRDef_RB->Value()==B_CONTROL_ON)
				list->AddItem(new BString("-R"));
			if (RRRat_RB->Value()==B_CONTROL_ON)
				list->AddItem(new BString("-r"));
		}
		if (HFS_CB->Value()==B_CONTROL_ON) {
			list->AddItem(new BString("-h"));
			if ((mappingCB->Value()==B_CONTROL_ON) && (strlen(mappingBF->GetTextControl()->Text())>0)) {
				list->AddItem(new BString("-map"));
				list->AddItem(new BString(mappingBF->GetTextControl()->Text()));
			}
		}
	}
	
	if (UDF_CB->Value()==B_CONTROL_ON) {
		list->AddItem(new BString("-udf"));
		if (UDFDVD_RB->Value()==B_CONTROL_ON)
			list->AddItem(new BString("-dvd-video"));
	}
	
	if (BFS_CB->Value()==B_CONTROL_ON) {
		list->AddItem(new BString("BFS"));
	}
	
	return list;
}


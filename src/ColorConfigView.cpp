#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str)	gMSHLangMgr->_T(str).String()
#define _TPS(str)	gMSHLangMgr->_T(str)
#else
#include <locale/Locale.h>
#endif

#include "ColorListItem.h"
#include "ColorConfigView.h"


#define LIST_SELECTION_MSG	'CCV0'
#define LOAD_DEFAULTS		'CCV1'


ColorConfigView::ColorConfigView(BRect frame) :
	ConfigView(frame)
{
	testCTV=new ColorTestView(BRect(15, 100, 80, 110), "ccv_testview");
	AddChild(testCTV);

	mainCC=new BColorControl(BPoint(15, 5), B_CELLS_32x8, 6, "ccv_colorcontrol", new BMessage(CTV_COLOR_UPDATE_MSG));
	AddChild(mainCC);
	//mainCC->ResizeTo(mainCC->Bounds().Width(), mainCC->Bounds().Height()+4);

	// list
	itemLV=new BListView(BRect(17, mainCC->Frame().bottom+7, frame.Width()-30, Bounds().bottom-7), "ccv_selectionlist", B_SINGLE_SELECTION_LIST, B_FOLLOW_ALL);
	AddChild((itemSV=new BScrollView("ccv_scrollview", itemLV, B_FOLLOW_LEFT | B_FOLLOW_TOP, 0, false, true)));
	
	AddItem(_T("Progress bar")); // "CLI:BURNING_STATUS_BAR_COLOR"
	AddItem(_T("CD size bar")); // "CLI:CDSIZE_STATUS_BAR_COLOR"
	AddItem(_T("Log window background")); // "CLI:LOGWIN_BG_COLOR"
	AddItem(_T("Log window text")); // "CLI:LOGWIN_FG_COLOR"
	AddItem(_T("File selection background")); // "CLI:LISTSELECTION_COLOR"
	AddItem(_T("File list background")); // "CLI:LISTBACKGROUND_COLOR"
	
	//AddChild((loaddefB=new BButton(BRect(0, 0, 10, 10), "ccv_defaultbutton", 
	//					_T("Load defaults"), // "B:Load Defaults"
	//					new BMessage(LOAD_DEFAULTS), B_FOLLOW_LEFT_RIGHT | B_FOLLOW_BOTTOM)));
	
}

ColorConfigView::~ColorConfigView() {
}

// PRIVATE

void	ColorConfigView::AllAttached() {

	BView::AllAttached();
	
	mainCC->ResizeTo( Bounds().Width() * 0.75, mainCC->Bounds().Height() );
	
	BTextControl	*red, *blue, *green;
	
	red=((BTextControl *)mainCC->FindView("_red"));
	green=((BTextControl *)mainCC->FindView("_green"));
	blue=((BTextControl *)mainCC->FindView("_blue"));
	
	//red->MoveTo(red->Frame().left, 0);
	//green->MoveTo(green->Frame().left, (mainCC->Bounds().Height()-green->Bounds().Height()+1)/2);
	//blue->MoveTo(blue->Frame().left , mainCC->Bounds().Height()-blue->Bounds().Height()+1);
	
	//red->SetLabel(_T("Red:")); // "ColorConfigView:Red:"
	//green->SetLabel(_T("Green:")); // "ColorConfigView:Green:"
	//blue->SetLabel(_T("Blue:")); // "ColorConfigView:Blue:"

	testCTV->MoveTo(mainCC->Frame().right+10, mainCC->Frame().top);
//	testCTV->ResizeTo( Bounds().right - 30, mainCC->Bounds().Height() );
	testCTV->ResizeTo(
		Bounds().right - mainCC->Frame().right - 30, mainCC->Bounds().Height());
	
	mainCC->SetTarget(this);
	
//	itemSV->ResizeTo((mainCC->Frame().Width()-red->Frame().Width())-5, Bounds().Height()-(mainCC->Frame().bottom+22));
//	itemSV->MoveTo(15, mainCC->Frame().bottom+7);

	itemLV->SetSelectionMessage(new BMessage(LIST_SELECTION_MSG));
	itemLV->SetTarget(this);
	
	mainCC->SetEnabled(false);
	//float x, y;
	//loaddefB->GetPreferredSize(&x, &y);
	//loaddefB->ResizeTo((Frame().right-itemSV->Frame().right)-14, y);
	//loaddefB->MoveTo(itemSV->Frame().right+7, itemSV->Frame().bottom-y);
}

rgb_color	ColorConfigView::GetColor(int32 index) {
	return ((ColorListItem *)itemLV->ItemAt(index))->GetColor();
}

void	ColorConfigView::SetColor(int32 index, rgb_color color) {
	((ColorListItem *)itemLV->ItemAt(index))->SetColor(color);
}

void	ColorConfigView::AddItem(const char *text) {
	itemLV->AddItem(new ColorListItem(text));
}

void	ColorConfigView::MessageReceived(BMessage *msg) {
	switch(msg->what) {
		case CTV_COLOR_UPDATE_MSG: {
			testCTV->SetTestColor(mainCC->ValueAsColor());
			if (itemLV->CurrentSelection()>=0) {
				((ColorListItem *)itemLV->ItemAt(itemLV->CurrentSelection()))->SetColor(mainCC->ValueAsColor());
				itemLV->InvalidateItem(itemLV->CurrentSelection());
			}
			break;
		}
		
		case LIST_SELECTION_MSG: {
			if (itemLV->CurrentSelection()>=0) {
				mainCC->SetEnabled(true);
				mainCC->SetValue(((ColorListItem *)itemLV->ItemAt(itemLV->CurrentSelection()))->GetColor());
				itemLV->InvalidateItem(itemLV->CurrentSelection());
				testCTV->SetTestColor(mainCC->ValueAsColor());
			} else
				mainCC->SetEnabled(false);

			break;
		}
		
		case LOAD_DEFAULTS: {
			
			break;
		}
		
		default: {
			BView::MessageReceived(msg);
			break;
		}
	}
}

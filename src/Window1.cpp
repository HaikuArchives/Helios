#ifndef _ZETA_USING_DEPRECATED_API_
#define _ZETA_USING_DEPRECATED_API_
#endif
	// To allow PostMessage() to work. Grrr...

#include <Application.h>
#include <Font.h>
#include <Directory.h>

#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str)	gMSHLangMgr->_T(str).String()
#define _TPS(str)	gMSHLangMgr->_T(str)
#else
#include <locale/Locale.h>
#endif

#include "Defines.h"
#include "ErrorBox.h"
#include "Window1.h"
#include "Application1.h"

Window1::Window1()
 : BWindow(BRect(150, 150, 150+600, 150+260), "", B_DOCUMENT_WINDOW, 0, B_CURRENT_WORKSPACE) {

	view1 = new View1(Bounds());
	AddChild(view1);
	number=0;
	
	view1->listview1->SetInvocationMessage(new BMessage('mm03'));
	view1->listview1->SetTarget(be_app_messenger);

	//view1->textcontrol2->SetTarget(be_app_messenger);

	SetTitle(_T("Helios")); // "TWINDOWTITLE"
	SetSizeLimits(600, 10000, 265, 10000);

	be_app->PostMessage('mm04');
}


Window1::~Window1(void) {
}


bool Window1::QuitRequested(void) {

	be_app->PostMessage(B_QUIT_REQUESTED);
	return false;
}


void Window1::MessageReceived(BMessage *message) {

	switch (message->what)
	{
		case B_SIMPLE_DATA: {		
			be_app->PostMessage(message);
			break;
		}
		
		case B_WORKSPACE_ACTIVATED: {
			Application1	*app=(Application1 *)be_app;
			
			app->mkisofs_output->SetWorkspaces(current_workspace());
			break;
		}

		default: {
			BWindow::MessageReceived(message);
			break;
		}	
	}
}


void Window1::WorkspaceActivated(int32 workspace, bool active) {
//	if (workspace==current_workspace())
}


bool Window1::Alert(char *text) {

	ErrorBox *alert=new ErrorBox(E_RED_COLOR, _T("Error"), text,_T("Abort"),_T("Continue"));
	return (alert->Go()==0);
}
/*
void Window1::Update_sizes() {

	BString str="";
	char *s="";
	size_t u;
	view1->box2->stringview5->SetHighColor(0,0,0,255);
	view1->box2->stringview10->SetHighColor(0,0,0,255);


	size_t r=(size_t)((size-((size_t)(size/1048576))*1048576)/10485.76);
	if (r<10) s="0"; else s="";

	str << (size_t)(size/1048576) << "." << s << r << "MB";
	view1->box2->stringview5->SetText(str.String());

	r=(size_t)(1.1484375*size);

	if ((size_t)(r/176400)>4440) {	// grösser als 74m
		view1->box2->stringview5->SetHighColor(100,0,0,255);
		view1->box2->stringview10->SetHighColor(100,0,0,255);
	}
	if ((size_t)(r/176400)>4800) {	// grösser als 80m
		view1->box2->stringview5->SetHighColor(150,0,0,255);
		view1->box2->stringview10->SetHighColor(150,0,0,255);
	}
	if ((size_t)(r/176400)>5400) {	// grösser als 91m
		view1->box2->stringview5->SetHighColor(200,0,0,255);
		view1->box2->stringview10->SetHighColor(200,0,0,255);
	}
	
	r=(size_t)(r/(176400*60));
	u=(size_t)((2352*size/(2048*176400))-(r*60));
	if (u<10) s="0"; else s="";
	str="";
	str << r << "m " << s << u << "s";
	view1->box2->stringview10->SetText(str.String());
}
*/


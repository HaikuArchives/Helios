#include <Application.h>

#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str) gMSHLangMgr->_T(str).String()
#define _TPS(str) gMSHLangMgr->_T(str)
#else
#include <locale/Locale.h>
#endif

#include "Defines.h"
#include "StatusWindow.h"

StatusWindow::StatusWindow()
	: BWindow(BRect(100, 100, 376, 160), _T("Helios"), // "TSTATUSWINDOWTITLE"
			  B_UNTYPED_WINDOW,
			  B_NOT_CLOSABLE | B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_NOT_H_RESIZABLE |
				  B_NOT_V_RESIZABLE | B_NOT_ANCHORED_ON_ACTIVATE | B_ASYNCHRONOUS_CONTROLS,
			  B_CURRENT_WORKSPACE)
{
	fStatusView = new StatusView();
	AddChild(fStatusView);
	SetLook(B_FLOATING_WINDOW_LOOK);
	SetFeel(B_MODAL_APP_WINDOW_FEEL);
	fStatusView->button17->SetTarget(this, this);
	interrupted = false;
}

StatusWindow::~StatusWindow()
{
}

bool StatusWindow::QuitRequested()
{
	Hide();
	return false;
}

void StatusWindow::SetBarColor(rgb_color color)
{
	fStatusView->progressSB->SetBarColor(color);
}

void StatusWindow::MessageReceived(BMessage* message)
{
	switch (message->what) {
	case KILL_MSG: {
		interrupted = true;
		if (Thread > 0) kill_thread(Thread);
		break;
	}

	default: {
		BWindow::MessageReceived(message);
		break;
	}
	}
}

void StatusWindow::Show()
{
	SetWorkspaces(B_CURRENT_WORKSPACE);
	BWindow::Show();
}

void StatusWindow::DisableControls()
{
	fStatusView->button17->SetEnabled(false);
}

void StatusWindow::EnableControls()
{
	fStatusView->button17->SetEnabled(true);
}

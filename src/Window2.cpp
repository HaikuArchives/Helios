#include <Application.h>
#include "Defines.h"
#include "Window2.h"
#include <String.h>

Window2::Window2()
	: BWindow(BRect(100, 100, 362, 164), "Rename item", B_MODAL_WINDOW,
			  B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE | B_NOT_H_RESIZABLE | B_NOT_V_RESIZABLE |
				  B_ASYNCHRONOUS_CONTROLS,
			  B_CURRENT_WORKSPACE)
{

	view2 = new View2();
	AddChild(view2);
	SetLook(B_FLOATING_WINDOW_LOOK);
	SetFeel(B_MODAL_APP_WINDOW_FEEL);
	shown = false;
}

Window2::~Window2(void)
{
}

bool Window2::QuitRequested(void)
{

	Hide();
	return false;
}

void Window2::MessageReceived(BMessage* message)
{

	switch (message->what) {

	case OK_MSG: {
		BString* str = new BString(view2->textcontrol3->Text());
		str->RemoveAll(" ");
		if ((str->Length() != 0) || (todo == 5)) switch (todo) {
			case 1: {
				be_app->PostMessage('mm11');
				break;
			}
			//				case 2: {be_app->PostMessage('mm12');break;}
			case 2: {
				be_app->PostMessage(NEW_FOLDER_MSG);
				break;
			}
			case 3: {
				be_app->PostMessage('mm13');
				break;
			}
			case 4: {
				be_app->PostMessage('mm14');
				break;
			}
			case 5: {
				be_app->PostMessage('mm15');
				break;
			}
			}
		delete str;
		Hide();
		break;
	}

	case CANCEL_MSG: {
		Hide();
		break;
	}

	default: {
		BWindow::MessageReceived(message);
		break;
	}
	}
}

void Window2::Show(void)
{

	shown = true;
	SetWorkspaces(B_CURRENT_WORKSPACE);
	BWindow::Show();
}

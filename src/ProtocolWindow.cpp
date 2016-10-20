#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>

#include <String.h>
#include <Entry.h>
#include <Path.h>
#include <NodeInfo.h>

#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str) gMSHLangMgr->_T(str).String()
#define _TPS(str) gMSHLangMgr->_T(str)
#else
#include <locale/Locale.h>
#endif

#include "FileDialog.h"
#include "Application1.h"
#include "LogSettingsWindow.h"
#include "ProtocolWindow.h"

#define LOG_CLEAR 'PW00'
#define ENABLE_LOG_FILE_MSG 'PW01'
#define SETLOGFILE_MSG 'PW02'
#define LOGSETTINGS_MSG 'PW03'

/*
BTextView(BRect frame, const char *name, BRect textRect,
      uint32 resizingMode, uint32 flags)
*/
// static sem_id		reader_sem;

ProtocolView::ProtocolView(BRect frame, const char* name, BRect textRect, uint32 resizingMode,
						   uint32 flags, int stream)
	: BTextView(frame, name, textRect, resizingMode, flags)
{
	fStream = stream;
}

void ProtocolView::AttachedToWindow()
{
	// ResizeTo(Window()->Bounds().Width(), Window()->Bounds().Height());
	BTextView::AttachedToWindow();
}

ProtocolView::~ProtocolView()
{
}

void ProtocolView::Pulse()
{

	if (fStream != -1) {
		long bytesread = 0;
		//		if (acquire_sem(reader_sem)!=B_NO_ERROR) return;
		if ((bytesread = read(fStream, fBuffer, 10000)) > 0) {
			// release_sem(reader_sem);
			fBuffer[bytesread] = 0;
			((ProtocolWindow*)this->Window())->AddText(fBuffer);
			//((ProtocolWindow *)this->Window())->UpdateIfNeeded();
		}
	}

	// BTextView::Pulse();
}

void ProtocolView::Start(int stream)
{
	fStream = stream;
	fcntl(fStream, F_SETFL, O_NONBLOCK);
}

int ProtocolView::Stop()
{
	int old = fStream;
	fStream = -1;
	return old;
}

ProtocolWindow::ProtocolWindow(BRect frame, const char* title, window_type type, int stream)
	: BWindow(frame, title, B_DOCUMENT_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL,
			  B_PULSE_NEEDED | B_ASYNCHRONOUS_CONTROLS)
{
	SetSizeLimits(300, 10000, 150, 10000);
	fStream = stream;
	mb = new BMenuBar(BRect(0, 0, frame.Width(), 1), _T("File"));			   // "TMI_FILE"
	mainM = new BMenu(_T("File"));											   // "TMI_FILE"
	clearMI = new BMenuItem(_T("Clear"), new BMessage(LOG_CLEAR), 'C');		   // "TMI_CLEAR"
	closeMI = new BMenuItem(_T("Close"), new BMessage(B_QUIT_REQUESTED), 'W'); // ""TMI_CLOSE"

	logfileM = new BMenu(_T("Log file"));				   // "TMI_LOG_FILE"
	enablefileloggingMI = new BMenuItem(_T("Log to file"), // "TMI_ENABLE_LOG_FILE"
										new BMessage(ENABLE_LOG_FILE_MSG), 'L');

	BString tmpstring;
	tmpstring << _T("Set file") << B_UTF8_ELLIPSIS; // "TMI_SETLOGFILENAME"
	setlognameMI = new BMenuItem(tmpstring.String(), new BMessage(SETLOGFILE_MSG));
	tmpstring = "";
	tmpstring << _T("Log settings") << B_UTF8_ELLIPSIS; // "TMI_SETLOGFILENAME"
	logsettingsMI = new BMenuItem(tmpstring.String(), new BMessage(LOGSETTINGS_MSG));

	contentView = new ProtocolView(
		BRect(0, 0, frame.Width() - B_V_SCROLL_BAR_WIDTH, frame.Height() - (B_H_SCROLL_BAR_HEIGHT)),
		"contentView", BRect(1, 1, 2000, 1000), B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP_BOTTOM,
		B_WILL_DRAW | B_PULSE_NEEDED, fStream);
	scrollView =
		new BScrollView("scrollView", contentView, B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP_BOTTOM, 0,
						true, true, B_NO_BORDER);
	AddChild(scrollView);
	AddChild(mb);
	mb->AddItem(mainM);
	mainM->AddItem(clearMI);
	mainM->AddItem(closeMI);

	mb->AddItem(logfileM);
	logfileM->AddItem(enablefileloggingMI);
	logfileM->AddItem(setlognameMI);
	////////////////////////////////////////////////
	// logfileM->AddItem(logsettingsMI);

	logfileF = new BFile();
	logfilenameS = "";
	datewritten = false;

	//	contentView=new BTextView(BRect(0,0,frame.Width()-B_V_SCROLL_BAR_WIDTH,
	//frame.Height()-B_H_SCROLL_BAR_HEIGHT), "contentView", frame,B_FOLLOW_ALL_SIDES, B_WILL_DRAW);
	//	scrollView=new BScrollView("scrollView", contentView, B_FOLLOW_ALL_SIDES, 0, true, true);
	SetPulseRate(0);
	//	reader_sem=create_sem(1, "ProtocolWindow(): reader_sem");
	SetLook(B_DOCUMENT_WINDOW_LOOK);
	contentView->MakeEditable(false);
	contentView->MakeResizable(false);
	contentView->SetStylable(true);

	MoveTo(40, 40);
	contentView->MakeFocus(true);
}

void ProtocolWindow::AllAttached()
{
}

bool ProtocolWindow::QuitRequested()
{
	if (IsHidden()) {
		MoveTo(100000, 100000);
		Show();
		Hide();
	}
	while (!IsHidden()) Hide();
	SetPulseRate(0);

// Wait until the app deals with this message before closing down.
// This is to allow GUI updates, etc.
#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
	BMessenger(be_app).SendMessage(PROTOCOL_WINDOW_CLOSED_MSG);
#else
	BMessage protocolWindowClosed(PROTOCOL_WINDOW_CLOSED_MSG);
	BMessenger(be_app).SendMessage(protocolWindowClosed);
#endif
	BWindow::QuitRequested();
	return false;
}

ProtocolWindow::~ProtocolWindow()
{
	//	delete_sem(reader_sem);
	logfileF->Unset();
	delete logfileF;
}

void ProtocolWindow::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
	case LOG_CLEAR: {
		Clear();
		break;
	}

	case ENABLE_LOG_FILE_MSG: {
		enablefileloggingMI->SetMarked(!enablefileloggingMI->IsMarked());
		// filelogging=enablefileloggingMI->IsMarked();
		setlognameMI->SetEnabled(enablefileloggingMI->IsMarked());
		if ((enablefileloggingMI->IsMarked()) && (strcmp(GetLogFileName(), "") == 0)) {
			// does not break. but goes on to the next "case" statement!
		} else
			break;
	}

	case SETLOGFILE_MSG: {
		BEntry* entry;
		BString folder, filename;

		folder = GetLogFileName();
		if (folder.Length() > 0) {
			folder.Remove(folder.FindLast('/') + 1, (folder.Length() - folder.FindLast('/')) - 1);
			filename = GetLogFileName();
			filename.RemoveFirst(folder.String());
		}
		FileDialog::fSourceWindow = this;
		entry = FileDialog::SaveDialog(_T("Log file"), folder.String(),
									   filename.String()); // "TMI_LOG_FILE"

		if (entry->InitCheck() == B_OK) {
			BPath path;
			entry->GetPath(&path);
			SetLogFileName(path.Path());
			path.Unset();
			// set MIME type to text/plain
			BNodeInfo* info = new BNodeInfo(logfileF);
			info->SetType("text/plain");
			delete info;
		}
		delete entry;
		//			logfileFP=new BFilePanel(B_SAVE_PANEL,new BMessenger(this), (const entry_ref *)NULL,
		//0UL, false,NULL, NULL, false, true);
		//			logfileFP->Window()->SetTitle(_T("Log file")); // "TMI_LOG_FILE"
		//			logfileFP->Show();
		break;
	}

	case B_CANCEL: {
		//			delete logfileFP;
		break;
	}

	case LOGSETTINGS_MSG: {
		LogSettingsWindow* settings =
			new LogSettingsWindow(BRect(0, 0, 300, 250), _T("Log settings")); // "L:Log Settings"
		settings->Show();
		break;
	}

	case B_SAVE_REQUESTED: {
		//			entry_ref	ref;
		//			BString		str;
		//			BEntry		entry;
		//			BPath		path;
		//
		//			if (msg->FindRef("directory", &ref)==B_OK)
		//				if (msg->FindString("name", &str)==B_OK) {
		//					entry.SetTo(&ref);
		//					entry.GetPath(&path);
		//					if (path.Path()[strlen(path.Path())-1]!='/')
		//						str.Prepend("/");
		//					str.Prepend(path.Path());
		//					SetLogFileName(str.String());
		//					path.Unset();
		//					entry.Unset();
		//
		//					// set MIME type to text/plain
		//					BNodeInfo	*info=new BNodeInfo(logfileF);
		//					info->SetType("text/plain");
		//					delete info;
		//
		//
		//				}
		break;
	}

	default: {
		BWindow::MessageReceived(msg);
		break;
	}
	}
}

void ProtocolWindow::Clear()
{
	Lock();
	contentView->SetText("");
	Unlock();
}

void ProtocolWindow::SetColors(rgb_color foreground, rgb_color background)
{
	contentView->SetViewColor(background);
	contentView->SetFontAndColor(be_plain_font, B_FONT_ALL, &foreground);
	contentView->Invalidate();
}

void ProtocolWindow::Show()
{
	SetWorkspaces(B_CURRENT_WORKSPACE);
	BWindow::Show();
	float width, height;

	Lock();
	mb->GetPreferredSize(&width, &height);
	scrollView->ResizeTo(Bounds().Width() + 1, Bounds().Height() - (height));
	mb->ResizeTo(Bounds().Width(), height);
	scrollView->MoveTo(0, height + 1);
	contentView->MakeFocus(true);
	Unlock();
	set_thread_priority(Thread(), 5); // priority change must happen after
									  // first Show(), because Show() sets
									  // priority to 15 at first call.
}

void ProtocolWindow::AddText(const char* string)
{
	Lock();

	if (DoesLogToFile()) {
		if (!datewritten) {
			// write current time into log file
			time_t t = time(NULL);
			BString str = "";

			str = "\n#\n# CURRENT DATE AND TIME: ";
			str.Append(asctime(localtime(&t))).Append("#\n\n");
			logfileF->Write(str.String(), str.Length());
			datewritten = true;
		}
		logfileF->Write(string, strlen(string));
	}

	BString* s = new BString(string);
	// s->Prepend(contentView->Text());
	s->ReplaceAll("\r", "\n");

	/*	int32 index=0;
		while ((index=(s->FindFirst("\r", index)))!=B_ERROR) {

			s->Remove(s->FindLast("\n",index)+1, ((s->FindFirst("\r",
	   index)+1))-(s->FindLast("\n",index)+1));
			index=(s->FindFirst("\r", index)+2);
		}
	*/
	while (s->FindFirst("\b") != B_ERROR) {
		if (s->FindFirst("\b") > 0) {
			s->Remove(s->FindFirst("\b") - 1, 1);
			s->RemoveFirst("\b");
		} else {
			s->RemoveFirst("\b");
			contentView->Delete(strlen(contentView->Text()) - 2, strlen(contentView->Text()) - 1);
		}
	}

	// contentView->SetText(s->String(), s->Length());
	contentView->Insert(strlen(contentView->Text()) - 1, s->String(), s->Length());
	delete s;
	contentView->ScrollToOffset(contentView->TextLength());
	// contentView->Sync();
	Unlock();
}

void ProtocolWindow::Start(int stream)
{
	contentView->Start(stream);
	SetPulseRate(1000000);
}

int ProtocolWindow::Stop()
{
	SetPulseRate(0);
	return contentView->Stop();
}

const char* ProtocolWindow::GetLogFileName()
{
	return strdup(logfilenameS.String());
}

void ProtocolWindow::SetLogFileName(const char* filename)
{
	logfileF->Unset();
	if (logfileF->SetTo(filename, B_READ_WRITE | B_CREATE_FILE | B_OPEN_AT_END) == B_OK)
		logfilenameS = strdup(filename);
	else {
		logfilenameS = "";
		enablefileloggingMI->SetMarked(false);
		setlognameMI->SetEnabled(false);
	}
}

bool ProtocolWindow::DoesLogToFile()
{
	return enablefileloggingMI->IsMarked();
}

void ProtocolWindow::SetLogToFile(bool state)
{
	enablefileloggingMI->SetMarked(state);
	setlognameMI->SetEnabled(enablefileloggingMI->IsMarked());
}

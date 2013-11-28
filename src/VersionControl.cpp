#include "VersionControl.h"

#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str)	gMSHLangMgr->_T(str).String()
#define _TPS(str)	gMSHLangMgr->_T(str)
#else
#include <locale/Locale.h>
#endif

#include <be/support/UTF8.h>
#include <stdlib.h>

#include "PipeCmd.h"

VersionControl::VersionControl(BRect frame, const char *name, uint32 resizingMode, uint32 flags)
		: BView(frame, name, resizingMode, flags) {

	fIcon=new BBitmap(BRect(0,0,15,15), B_CMAP8);
	fString=new BString();
	SetViewColor(ui_color( B_PANEL_BACKGROUND_COLOR ));
	SetFont(be_plain_font);
	SetFontSize(10);
}

VersionControl::~VersionControl() {
	if (fIcon)
		delete fIcon;
	delete fString;
}

void	VersionControl::SetIcon(BBitmap *icon) {
	if (fIcon)
		delete fIcon;
	fIcon=icon;
	Draw(Bounds());
}

void	VersionControl::SetText(const char *text) {
	fString->SetTo(text);
	Draw(Bounds());
}


void	VersionControl::Draw(BRect updateRect) {
	SetDrawingMode(B_OP_OVER);
	DrawBitmap(fIcon, BPoint(0,0));
	MovePenTo(fIcon->Bounds().Width()+5, Bounds().Height()-4);
	SetHighColor(0,0,0);
	SetLowColor(255,255,255);
	DrawString(fString->String());
	BView::Draw(updateRect);
}

const char *
VersionControl::GetToolVersion(const char *path)
{
	thread_id	thread;
	int		in, out, err;
	const char	**argv;
	int		argc=0;
	const char	*threadname="Helios:GetToolVersion()";
	char		buffer[1000];
	size_t		bytesread;
	BString		version="";

	buffer[0]=0;
	argv=(const char **)malloc(sizeof(char *) * (5));
	argv[argc++]=strdup(path);
	argv[argc++]=strdup("--version");
	argv[argc]=NULL;

	thread=pipe_command(argc, argv, in, out, err);
	rename_thread(thread, threadname);
	resume_thread(thread);


	while ((bytesread=read(err,(void *)buffer, 1000))>0) {
		buffer[bytesread]=0;
		version.Append(buffer);
		//printf("%s: %s\n",path, buffer);
		snooze(10000);
	}
	while ((bytesread=read(out,(void *)buffer, 1000))>0) {
		buffer[bytesread]=0;
		version.Append(buffer);
		//printf("%s: %s\n",path, buffer);
		snooze(10000);
	}

	free(argv);
	close(in);
	close(out);
	close(err);
	
	
	char 	str[1024];
	int32	length=1024;
	int32	state=0;
	int32	srclen=version.Length();
	convert_to_utf8(B_ISO1_CONVERSION, version.String(), &srclen, str, &length, &state);
	
	str[length]=0;
	version=str;
	
	// --version option not supported?
	if (version.FindFirst("--version") != B_ERROR) {
		version=path;
		version << ": " << _T("(could not get version info)"); // "STR:(could not get version info)"
	} else {
		version.RemoveAll("\n");
		version.RemoveAll("\t");
		// for case-insensitivity replace instead of remove...
		version.IReplaceAll("Copyright ", "");
		version.IReplaceAll("(c) ", B_UTF8_COPYRIGHT);
		version.IReplaceAll("(c)", B_UTF8_COPYRIGHT);
	}
	return strdup(version.String());
}

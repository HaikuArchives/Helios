#ifndef _PROTOCOLWINDOW_H_
#define _PROTOCOLWINDOW_H_

#include <Window.h>
#include <TextView.h>
#include <ScrollView.h>
#include <Rect.h>
#include <MenuBar.h>
#include <Menu.h>
#include <MenuItem.h>
#include <OS.h>
#include <String.h>
#include <FilePanel.h>
#include <File.h>

// Message for other classes to intercept,
// for when the window is closed by the user.
#define PROTOCOL_WINDOW_CLOSED_MSG	'PWCD'

class ProtocolView : public BTextView {
public:
	ProtocolView(BRect frame, const char *name, BRect textRect, uint32 resizingMode, uint32 flags, int stream);
	~ProtocolView();
	
	void 	Start(int stream);
	int	Stop();
	
private:
	void Pulse();
	
	int 	fStream;
	char 	fBuffer[10000];
	
protected:
	void	AttachedToWindow();
};

class ProtocolWindow : public BWindow {
public:
	ProtocolWindow(BRect frame, const char *title, window_type type, int stream);
	~ProtocolWindow();
	
	void	Clear();
	void	AddText(const char *string);
	void 	Start(int stream);
	int	Stop();

	const char *GetLogFileName();
	void	SetLogFileName(const char *);
	
	bool	DoesLogToFile();
	void	SetLogToFile(bool);

	void			SetColors(rgb_color foreground, rgb_color background);
	virtual void	Show();
	virtual void MessageReceived(BMessage *msg);
	virtual void AllAttached();
	
private:
	bool	QuitRequested();
	
	ProtocolView	*contentView;
	BScrollView	*scrollView;
	int		fStream;
	BMenu		*mainM,
			*logfileM;
	BMenuItem	*clearMI,
			*closeMI,
			*enablefileloggingMI,
			*setlognameMI,
			*logsettingsMI;
	BFile		*logfileF;
	BString		logfilenameS;
	BFilePanel	*logfileFP;
	bool		datewritten;
	BMenuBar	*mb;
};



#endif

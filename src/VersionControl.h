#ifndef _VERSIONCONTROL_H_
#define _VERSIONCONTROL_H_

#include <InterfaceKit.h>
#include <String.h>

class VersionControl : public BView {
public:
	static const char*			GetToolVersion(const char *path);
														// Pass in the path to the tool in question.
														// Returns the version details.
		
													VersionControl(BRect frame, const char *name, uint32 resizingMode, uint32 flags);
													~VersionControl();
		
	void										SetIcon(BBitmap *);
	void										SetText(const char *);		

protected:
	virtual void						Draw(BRect updateRect);
	
	private:
		BBitmap*							fIcon;
		BString*							fString;
};

#endif
// _VERSIONCONTROL_H_

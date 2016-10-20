#ifndef _CONFIGHEADLINE_H_
#define _CONFIGHEADLINE_H_

#include <View.h>
#include <String.h>


class ConfigHeadLine : public BView {
public:
	ConfigHeadLine(BRect frame, const char *name, uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP, uint32 flags = B_WILL_DRAW);
	~ConfigHeadLine();
	
	void	SetText(const char *);
	const char *Text();
	
	virtual void Draw(BRect updateRect);
	
private:
	BString		fText;
};


#endif

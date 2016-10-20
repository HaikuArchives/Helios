#ifndef _STATUSBAR_H_
#define _STATUSBAR_H_

#include <InterfaceKit.h>
#include <String.h>

class StatusBar : public BView
{
public:
	StatusBar(BRect, const char*, int32, uint32);
	~StatusBar();

	void SetText(const char*, uint32 delay = 0L);

protected:
	virtual void Draw(BRect);
	void DrawLabel(rgb_color);

private:
	BString fText;
	rgb_color currentRGB;
	uint32 fDelay;
	static int32 delay_and_fade(void* data);
};

#endif

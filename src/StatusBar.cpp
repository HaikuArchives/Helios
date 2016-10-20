#include "StatusBarView.h"

StatusBar::StatusBar(BRect frame, const char* name, int32 resizingMode, uint32 flags)
	: BView(frame, name, resizingMode, flags)
{
	SetViewColor(216, 216, 216);
	fText = "";
	fDelay = 0;
	SetFontSize(9);
	currentRGB = (rgb_color){0, 0, 0};
}

StatusBar::~StatusBar()
{
}

void StatusBar::Draw(BRect updateRect)
{
	BRect frame = Bounds(), f2;

	f2 = frame;
	f2.InsetBy(1, 1);
	f2.top++;
	SetHighColor(216, 216, 216);
	FillRect(f2);
	SetHighColor(255, 255, 255);
	StrokeLine(BPoint(0, 1), BPoint(frame.right - 1, 1));
	StrokeLine(BPoint(0, 1), BPoint(0, frame.bottom));
	SetHighColor(150, 150, 150);
	StrokeLine(BPoint(0, 0), BPoint(frame.right, 0));
	// StrokeLine(BPoint(0, frame.bottom), BPoint(frame.right, frame.bottom));
	StrokeLine(BPoint(frame.right, 1), BPoint(frame.right, frame.bottom));

	DrawLabel(currentRGB);
}

void StatusBar::DrawLabel(rgb_color color)
{
	BRect frame = Bounds();
	BString s2 = fText, s = fText;

	while (StringWidth(s2.String()) > frame.Width() - 10) {
		s.Truncate(s.Length() - 1);
		s2 = s;
		s2.Append("...");
	}

	SetHighColor(color);
	SetLowColor(216, 216, 216);
	DrawString(s2.String(), BPoint(5, frame.bottom - 2));
}

int32 StatusBar::delay_and_fade(void* data)
{
	StatusBar* sb = (StatusBar*)data;

	snooze(sb->fDelay);

	for (int32 i = 0; i <= 216; i += 4) {
		if (sb->Window()->Lock()) {
			sb->currentRGB = (rgb_color){i, i, i};
			sb->DrawLabel(sb->currentRGB);
			sb->Window()->Unlock();
		}
		snooze(80000);
	}
	sb->fText = "";
	sb->currentRGB = (rgb_color){0, 0, 0};
	return 0;
}

void StatusBar::SetText(const char* text, uint32 delay)
{
	static thread_id thread = 0;

	fText = text;
	fDelay = delay;
	currentRGB = (rgb_color){0, 0, 0};
	if (thread != 0) {
		kill_thread(thread);
		thread = 0;
	}
	if (fDelay > 0) {
		if (delay > 0)
			resume_thread((thread = spawn_thread(delay_and_fade, "StatusBar Text-Fader", 2, this)));
	}
	Invalidate();
}

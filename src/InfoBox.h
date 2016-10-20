#ifndef _INFOBOX_H_
#define _INFOBOX_H_

#include <Box.h>
#include <StringView.h>
#include <Font.h>
#include "Defines.h"
#include <OS.h>
#include <Directory.h>
#include <StatusBar.h>
#include <Button.h>

class InfoBox : public BBox
{
public:
	InfoBox();
	~InfoBox(void);

	BStringView* stringview3;
	BStringView* stringview4;
	BStringView* stringview5;
	BStringView* stringview6;
	BStringView* stringview9;
	BStringView* stringview10;
	BStringView* filesSV, *foldersSV;
	BStringView* filecounterSV, *foldercounterSV;
	BButton* button3;

	void Refresh();
	void SpawnUpdateThread();
	void ExitUpdateThread();
	void SetUpdateFrequency(int32 seconds);
	void SetCDSize(size_t size);
	size_t GetCDSize();
	void StopUpdating();
	void StartUpdating();
	void SetBarColor(rgb_color color);

	void DrawWarningBitmap();
	virtual void AttachedToWindow();

private:
	void UpdateSizes();
	static int32 UpdateThread(void* data);
	void GetFolder(BDirectory dir);
	void Draw(BRect);

	size_t cd_size;
	int folders, files;
	thread_id update_Thread;
	bool ut_running;

	BStatusBar* levelSB;
	BBitmap* warningBM;
};

#endif


#ifndef _AUDIOROW_H_
#define _AUDIOROW_H_

#include <ListItem.h>
#include <String.h>
#include "ColumnListView.h"
#include "ColumnTypes.h"
#include <Entry.h>

class AudioRow : public BRow
{
public:
	AudioRow(int32 track, const char* text, int32 pause, off_t size, off_t length);
	// AudioRow(track, title, pregap_frames, size_bytes, length_seconds)
	// AudioRow(int32 track, const char *text, int32 pause, off_t size);
	virtual ~AudioRow();

	void SetBytes(off_t bytes);
	off_t GetBytes();

	void SetLength(off_t seconds);
	off_t GetLength();

	void SetPregap(int32 frames);
	int32 GetPregap();

	void SetFilename(const char* leaf);
	const char* GetFilename();

	bool IsSplitted();
	void SetSplitted(bool splitted);

	int32 GetSplitIndex();
	void SetSplitIndex(int32 index);

	bool IsListCorrect();
	void SetIndexList(BList* list);
	BList* GetIndexList();
	const char* GetCdrecordReadyList();

	const char* GetCDTitle();
	void SetCDTitle(const char* title);

	void SetTrackNumber(int32 track);
	int32 GetTrackNumber();

	void SetStartTime(bigtime_t);
	bigtime_t GetStartTime();

	void SetEndTime(bigtime_t);
	bigtime_t GetEndTime();

	void SetEntry(BEntry*);
	const BEntry* GetEntry();

	void SetStartFrame(int64);
	int64 GetStartFrame();

	void SetEndFrame(int64);
	int64 GetEndFrame();

private:
	const char* Format(off_t seconds);
	const char* GetUserList();

	BStringField* nameFIELD, *lengthFIELD, *indexFIELD, *pregapFIELD, *cdtextFIELD;
	BSizeField* bytesFIELD;
	BIntegerField* trackFIELD;
	bool splitted;
	int32 splitindex;
	int32 pregapframes;
	off_t tracklength;
	int32 kList[99], kLastList[99];
	int kListc;
	BList* indexL;

	BEntry* fEntry;
	bigtime_t startT, endT;
	int64 startF, endF;
};

#endif

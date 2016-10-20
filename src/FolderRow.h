#ifndef _FOLDERROW_H_
#define _FOLDERROW_H_

#include <ListItem.h>
#include <String.h>
#include "ColumnListView.h"
#include "ColumnTypes.h"
#include <Bitmap.h>
#include "FileField.h"
#include "FileColumn.h"

#define FF_ELTORITOIMAGE 1

class FolderRow : public BRow
{
public:
	FolderRow(const char* text, bool folder, BBitmap* icon = NULL, bool parent = false);
	FolderRow(const char* text, off_t size, bool folder, BBitmap* icon = NULL, bool parent = false);
	virtual ~FolderRow();

	void SetSize(off_t size);
	off_t GetSize();

	void SetBitmap(BBitmap* bmap);
	BBitmap* GetBitmap();

	void SetFilename(const char* text);
	const char* GetFilename();

	void SetFlags(int32 flags);
	int32 GetFlags();
	bool HasFlag(int32 flag);

	bool IsFolder();
	bool IsFile();

private:
	BFileField* nameFIELD;
	BSizeField* sizeFIELD;
	BBitmapField* iconFIELD;
	BBitmap* iconB;
	bool kfolder;
	BString filenameS;
	int32 cFlags;
};

#endif

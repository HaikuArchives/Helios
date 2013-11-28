#include "FolderRow.h"
#include "Defines.h"
#include <stdio.h>

FolderRow::FolderRow(const char *text, bool folder, BBitmap *icon, bool parent):BRow(17.0){

	nameFIELD=new BFileField(text, folder);
	nameFIELD->SetParentFolder(parent);
	sizeFIELD=new BSizeField(0);
	filenameS=text;
	cFlags=0;
	
//	if (icon==NULL)
//		iconB=new BBitmap(BRect(0.0,0.0,15.0,15.0), B_CMAP8);
//	else
//		if ((icon->Bounds().Width()!=16) || (icon->Bounds().Height()!=16))
//			iconB=new BBitmap(BRect(0.0,0.0,15.0,15.0), B_CMAP8);
//		else
			iconB=icon;
	iconFIELD=new BBitmapField(iconB);
	SetField(iconFIELD, ICON_FIELD);
	SetField(nameFIELD, FILENAME_FIELD);
	if (!folder) SetField(sizeFIELD, SIZE_FIELD);
	kfolder=folder;
	
	if (filenameS.Compare("..")==0)
		nameFIELD->SetEditable(false);
	else
		nameFIELD->SetEditable(true);
}

FolderRow::FolderRow(const char *text, off_t size, bool folder,BBitmap *icon, bool parent):BRow(17.0) {

	nameFIELD=new BFileField(text, folder);
	nameFIELD->SetParentFolder(parent);
	sizeFIELD=new BSizeField(size);
	filenameS=text;
	cFlags=0;

//	if (icon==NULL)
//		iconB=new BBitmap(BRect(0.0,0.0,15.0,15.0), B_CMAP8);
//	else
//		if ((icon->Bounds().Width()!=16) || (icon->Bounds().Height()!=16))
//			iconB=new BBitmap(BRect(0.0,0.0,15.0,15.0), B_CMAP8);
//		else
			iconB=icon;
	iconFIELD=new BBitmapField(iconB);
	SetField(iconFIELD, ICON_FIELD);
	SetField(nameFIELD, FILENAME_FIELD);
	if (!folder) SetField(sizeFIELD, SIZE_FIELD);
	kfolder=folder;
	
	if (filenameS.Compare("..")==0)
		nameFIELD->SetEditable(false);
	else
		nameFIELD->SetEditable(true);
}

FolderRow::~FolderRow() {
}

bool FolderRow::IsFolder() {
	return kfolder;
}

bool FolderRow::IsFile() {
	return !kfolder;
}

void FolderRow::SetSize(off_t size) {
	sizeFIELD->SetSize(size);
}

off_t FolderRow::GetSize() {
	return sizeFIELD->Size();
}

void FolderRow::SetBitmap(BBitmap *bmap) {
	if (iconB!=NULL) delete iconB;
	iconB=bmap;
	iconFIELD->SetBitmap(iconB);
}

BBitmap *FolderRow::GetBitmap() {
	BMessage	archive;
	
	if (iconB->Archive(&archive)==B_OK)
		return new BBitmap(&archive);
	else
		return NULL;
}

void FolderRow::SetFilename(const char *text) {
	filenameS=text;
	nameFIELD->SetString(text);
}

const char *FolderRow::GetFilename() {
	//printf("HELIOS: GetFilename()=%s\n", filenameS.String());
	return filenameS.String();
}

void FolderRow::SetFlags(int32 flags) {
	cFlags=flags;
}

int32 FolderRow::GetFlags() {
	return cFlags;
}

bool FolderRow::HasFlag(int32 flag) {
	return (cFlags & flag)==flag;
}

#include "M3UList.h"
#include <File.h>
#include <Entry.h>
#include <string.h>
#include <String.h>
#include <stdio.h>

M3UList::M3UList():BList() {
	
}


M3UList::~M3UList() {
}


bool M3UList::AddItem(BString *filename) {
	return BList::AddItem((void *)filename->String());
}


bool M3UList::AddItem(const char *filename) {
	return BList::AddItem((void *)filename);
}


bool M3UList::AddItem(const char *filename, int32 index) {
	return BList::AddItem((void *)filename, index);
}


bool M3UList::RemoveItem(const char *filename) {
	return BList::RemoveItem((void *)filename);
}


char *M3UList::RemoveItem(int32 index) {
	char *retval=(char *)BList::RemoveItem(index);
	return retval;
}


char *M3UList::LastItem() {
	char *retval=(char *)BList::LastItem();
	return retval;
}


char *M3UList::FirstItem() {
	char *retval=(char *)BList::FirstItem();
	return retval;
}


char *M3UList::ItemAt(int32 index) {
	char *retval=(char *)BList::ItemAt(index);
	return retval;
}


void M3UList::SaveToFile(BEntry *entry) {
	BString line;
	BFile	*file=new BFile(entry, B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);

	//if (!entry->Exists()) {delete file; return;}
	for (int32 index=0; index < this->CountItems(); index++) {
		line.SetTo(this->ItemAt(index)).Append("\n");
		file->Write((void *)line.String(), line.Length());
	}
	file->Unset();
	delete file;
}


void M3UList::LoadFromFile(BEntry *entry) {
	BFile	*file=new BFile(entry, B_READ_ONLY);
	char	*buffer="";
	char	*eol="\n";
	BString	line="";
	
	while (true) {
		do {
			file->Read(buffer, 1);
			line.Append(line).Append(buffer);
		} while (strcmp(buffer, eol)!=0);
		line="";
	}
	file->Unset();
	delete file;
}


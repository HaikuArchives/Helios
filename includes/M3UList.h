#ifndef _M3ULIST_H_
#define _M3ULIST_H_

#include <List.h>
#include <String.h>
#include <storage/Entry.h>

class M3UList : BList {

public:
	M3UList();
	~M3UList();
	
	bool AddItem(BString *filename);
	bool AddItem(const char *filename);
	bool AddItem(const char *filename, int32 index);
	bool RemoveItem(const char *filename);
	char *RemoveItem(int32 index);
	char *ItemAt(int32 index);
	char *LastItem();
	char *FirstItem();
	
	void SaveToFile(BEntry *entry);
	void LoadFromFile(BEntry *entry);
	
};


#endif

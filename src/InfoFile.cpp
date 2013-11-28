#include <stdio.h>

#include <storage/Path.h>

#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str)	gMSHLangMgr->_T(str).String()
#define _TPS(str)	gMSHLangMgr->_T(str)
#else
#include <locale/Locale.h>
#endif

//#include "Stringtable.h"
#include "InfoFile.h"

InfoFile::InfoFile(const char *filename) {
	fEntry=new BFile(filename, B_CREATE_FILE | B_ERASE_FILE | B_READ_WRITE);
	entry=new BEntry(filename, true);
	printf("FILENAME=%s\n", filename);
}

InfoFile::~InfoFile() {
	fEntry->Unset();
	entry->Unset();
	delete entry;
	delete fEntry;
}

void		InfoFile::SetDiscID(const char *cdindex_id, uint64 cddb_id) {
	fCDIndex_ID=cdindex_id;
	fCDDB_ID=cddb_id;
}

void		InfoFile::SetMCN() {
}

void		InfoFile::SetISRC(const char *isrc) {
	fISRC=isrc;
}


void		InfoFile::SetPerformer(const char *text) {
	sprintf(fPerformer, "%s", text);
}

void		InfoFile::SetAlbumtitle(const char *text) {
	sprintf(fAlbumtitle, "%s", text);
}

void		InfoFile::SetTracktitle(const char *text) {
	sprintf(fTracktitle, "%s", text);
}


const char	*InfoFile::GetPath() {
	BPath	p;
	entry->GetPath(&p);
	return strdup(p.Path());
}

const char 	*InfoFile::GetLeaf() {
	BPath	p;
	entry->GetPath(&p);
	return strdup(p.Leaf());
}

void		InfoFile::Write() {
	char	line[200];
	
	sprintf(line, "# created by %s %s\n#\n", _T("ABOUT_APPLICATIONNAME"), _T("ABOUT_VERSIONNUMBER"));
	fEntry->Write((void *)(&line), strlen(line));
	sprintf(line, "CDINDEX_DISCID=\t'%s'\n", fCDIndex_ID.String());
	fEntry->Write((void *)(&line), strlen(line));
	sprintf(line, "CDDB_DISCID=\t0x%llx\n", fCDDB_ID);
	fEntry->Write((void *)(&line), strlen(line));
	sprintf(line, "MCN=\t\t\n");
	fEntry->Write((void *)(&line), strlen(line));
	sprintf(line, "ISRC=\t\t%s\n#\n", fISRC.String());
	fEntry->Write((void *)(&line), strlen(line));
	sprintf(line, "Performer=\t'%s'\n", fPerformer);
	fEntry->Write((void *)(&line), strlen(line));
	sprintf(line, "Albumtitle=\t'%s'\n", fAlbumtitle);
	fEntry->Write((void *)(&line), strlen(line));
	sprintf(line, "Tracktitle=\t'%s'\n", fTracktitle);
	fEntry->Write((void *)(&line), strlen(line));
	fEntry->Sync();
}





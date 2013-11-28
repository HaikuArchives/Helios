#ifndef _INDEXFILE_H_
#define _INDEXFILE_H_

#include <storage/Entry.h>
#include <storage/Path.h>
#include <String.h>
#include <storage/File.h>

// InfoFile can write only but not read .inf-files

class InfoFile {
	public:
		InfoFile(const char *filename);
		~InfoFile();
	
		void		SetDiscID(const char *cdindex_id, uint64 cddb_id);
		void		SetMCN();
		void		SetISRC(const char *isrc);
		
		// CD-Text
		void		SetPerformer(const char *);
		void		SetAlbumtitle(const char *);
		void		SetTracktitle(const char *);

		const char	*GetPath();		// full path
		const char 	*GetLeaf();		// leaf only
		
		void		Write();

	private:
		BFile		*fEntry;
		BEntry		*entry;
		BString		fName;
		BString		fCDIndex_ID,
				fISRC;
		uint64		fCDDB_ID;
		char		fPerformer[160];
		char		fAlbumtitle[160];
		char		fTracktitle[160];
		
};


#endif


#ifndef _CDTEXT_H_
#define _CDTEXT_H_

#include <File.h>
#include <List.h>
#include <String.h>

struct textpack {
	uchar	pack_type;	/* Pack Type indicator	*/
	char	track_no;	/* Track Number (0..99)	*/
	char	seq_number;	/* Sequence Number	*/
	char	block_number;	/* Block # / Char pos	*/
	char	text[12];	/* CD-Text Data field	*/
	char	crc[2];		/* CRC 16		*/
};

struct list {
	uchar	type;
	char	track_no;
	char	text[161];
};

class CDTextFile : public BFile {
	public:
		CDTextFile(const BEntry *entry);
		CDTextFile(const char *path, uint32 openMode);
		~CDTextFile();
		
		void FlushAll();
		void SetAlbum(const char *text);
		const char *GetAlbum();
		void SetPerformer(int track, const char *text);
		const char *GetPerformer(int);
		void SetTrack(int track, const char *text);
		const char *GetTrack(int track);
		
		void SetCharacterCode(unsigned char code);
		void SetLanguage(unsigned char code);
		void SetTracks(unsigned char);
		
	private:

		unsigned char	GetPackCount(unsigned char code);
		status_t	read_header();
		textpack	*write_blocks();
		
		uchar	hbuf[4];		// keeps information about size of contained data
		BList	*dataL;
		BList	*stringL;
		
		// formerly static stuff
		list	*l;
		int32	listno;
		uint 	strpos;
		int	seq_num;
		unsigned char	ccode,
					fLanguage,
					fTracks;
		
};


#endif

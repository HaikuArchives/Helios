#include <stdio.h>
#include <stdlib.h>

#include "crc16.h"

#include "CDText.hpp"

#define PTI_TITLE 0x80		 /* Album name and Track titles */
#define PTI_PERFORMER 0x81   /* Singer/player/conductor/orchestra */
#define PTI_SONGWRITER 0x82  /* Name of the songwriter */
#define PTI_COMPOSER 0x83	/* Name of the composer */
#define PTI_ARRANGER 0x84	/* Name of the arranger */
#define PTI_MESSAGE 0x85	 /* Message from content provider or artist */
#define PTI_DISK_ID 0x86	 /* Disk identification information */
#define PTI_GENRE 0x87		 /* Genre identification / information */
#define PTI_TOC 0x88		 /* TOC information */
#define PTI_TOC2 0x89		 /* Second TOC */
#define PTI_RES_8A 0x8A		 /* Reserved 8A */
#define PTI_RES_8B 0x8B		 /* Reserved 8B */
#define PTI_RES_8C 0x8C		 /* Reserved 8C */
#define PTI_CLOSED_INFO 0x8D /* For internal use by content provider */
#define PTI_ISRC 0x8E		 /* UPC/EAN code of album and ISRC for tracks */
#define PTI_SIZE 0x8F		 /* Size information of the block */

/*

size block:
0  1  2  3  00 01 02 03 04 05 06 07 08 09 10 11 CRC16
8F 00 17 00 _a _b _c _d _e _f _g _h _i _j _k _l xxxx
8F 01 18 00 _m _n _o _p _q _r _s _t _u 00 00 00 yyyy
8F 02 19 00 00 00 00 00 _v 00 00 00 00 00 00 00 zzzz


_a	character code (see table 00h)
_b	first track number
_c	last track number
_d	copyright (=3)
_e	pack count (80)
_f	pack count (81)
_g	pack count (82)
_h	pack count (83)
_i	pack count (84)
_j	pack count (85)
_k	pack count (86)
_l	pack count (87)
_m	pack count (88)
_n	pack count (89)
_o	pack count (8A)
_p	pack count (8B)
_q	pack count (8C)
_r	pack count (8D)
_s	pack count (8E)
_t	pack count (8F)
_u	last sequence number
_v	language (see table 01h)


table 00h	charsets
-----------------------------------
0x00		ISO 8859-1
0x01		ISO 646, ASCII (7 bit)
0x02		Reserved codes 0x02..0x7f
0x80		Music Shift-JIS Kanji
0x81		Korean
0x82		Mandarin Chinese
0x83		Reserved codes 0x83..0xFF


table 01h	languages
-----------------------------------
0x06		Czech
0x07		Danish
0x08		German
0x09		English
0x0A		Spanish
0x0F		French
0x15		Italian
0x1B		Hungarian
0x1D		Dutch
0x1E		Norwegian
0x20		Polish
0x21		Portuguese
0x26		Slovene
0x27		Finnish
0x28		Swedish
0x56		Russian
0x65		Korean
0x69		Japanese
0x70		Greek
0x75		Chinese



table 02h	genre
-----------------------------------
0		not used
1		not defined
2		Adult Contemporary
3		Alternative Rock
4		Childrens Music
5		Classical
6		Contemporary Christian
7		Country
8		Dance
9		Easy Listening
10		Erotic
11		Folk
12		Gospel
13		Hip Hop
14		Jazz
15		Latin
16		Musical
17		New Age
18		Opera
19		Operetta
20		Pop Music
21		RAP
22		Reggae
23		Rock Music
24		Rhythm & Blues
25		Sound Effects
26		Spoken Word
28		World Music
29		Reserved is 29..32767
32768		Registration by RIAA 32768..65535



*/

CDTextFile::CDTextFile(const BEntry* entry) : BFile(entry, B_READ_WRITE)
{
	dataL = new BList();
	stringL = new BList();
}

CDTextFile::CDTextFile(const char* path, uint32 openMode) : BFile(path, openMode)
{
	dataL = new BList();
	stringL = new BList();
}

CDTextFile::~CDTextFile()
{
	delete dataL;
	delete stringL;
}

void CDTextFile::SetCharacterCode(unsigned char code)
{
	ccode = code;
}

void CDTextFile::SetLanguage(unsigned char code)
{
	fLanguage = code;
}

void CDTextFile::SetTracks(unsigned char tracks)
{
	fTracks = tracks;
}

unsigned char CDTextFile::GetPackCount(unsigned char code)
{
	unsigned char count = 0;

	for (int i = 0; i < dataL->CountItems(); i++) {
		if (((textpack*)dataL->ItemAt(i))->pack_type == code) count++;
	}

	return count;
}

textpack* CDTextFile::write_blocks()
{
	struct textpack* tp = new textpack();
	static list* l;

	union _c {
		uint16 ui;
		char c[2];
	} c;

	l = (list*)(stringL->ItemAt(listno));
	if (l == NULL) return NULL;

	if ((strpos > strlen(l->text)) || (l == NULL)) {
		listno++;
		l = (list*)(stringL->ItemAt(listno));
		if (l == NULL) return NULL;
		strpos = 0;
	}
	// l=(list *)(stringL->ItemAt(listno++));

	tp->pack_type = l->type;
	tp->track_no = l->track_no;
	tp->seq_number = seq_num++;
	tp->block_number = (strpos > 15) ? 15 : strpos;

	for (int i = 0; i < 12; i++) tp->text[i] = 0;

	for (int i = 0; i < 12; i++) {
		if (strpos > strlen(l->text)) {
			l = (list*)(stringL->ItemAt(++listno));
			if (l == NULL) break;
			strpos = 0;
		}
		tp->text[i] = l->text[strpos++];
	}
	// cdtext_crc_ok(tp);
	c.ui = fillcrc((uchar*)(tp), sizeof(*tp));
	dataL->AddItem(tp);
	// c.ui=flip_crc_error_corr((uchar *)tp, sizeof(*tp)-2, c.ui);
	// c.ui=fillcrc((uchar *)tp, sizeof(*tp));
	// tp->crc[0]=c.c[0];
	// tp->crc[1]=c.c[1];

	return tp;
}

void CDTextFile::FlushAll()
{
	textpack* tp;
	size_t filesize = 0;

	listno = 0;
	strpos = 0;
	seq_num = 0;

	Write((void*)hbuf, 4);
	while ((tp = write_blocks()) != NULL) {
	}

	// 3 size block
	tp = new textpack();
	tp->pack_type = PTI_SIZE;
	tp->track_no = 0;
	tp->seq_number = seq_num++;
	tp->block_number = 0;
	tp->text[0] = ccode;			   // charset
	tp->text[1] = 1;				   // first track number
	tp->text[2] = fTracks;			   // last track number
	tp->text[3] = 3;				   // copyright?
	tp->text[4] = GetPackCount(0x80);  // pack count
	tp->text[5] = GetPackCount(0x81);  // pack count
	tp->text[6] = GetPackCount(0x82);  // pack count
	tp->text[7] = GetPackCount(0x83);  // pack count
	tp->text[8] = GetPackCount(0x84);  // pack count
	tp->text[9] = GetPackCount(0x85);  // pack count
	tp->text[10] = GetPackCount(0x86); // pack count
	tp->text[11] = GetPackCount(0x87); // pack count
	fillcrc((uchar*)(tp), sizeof(*tp));
	dataL->AddItem(tp);

	tp = new textpack();
	tp->pack_type = PTI_SIZE;
	tp->track_no = 1;
	tp->seq_number = seq_num++;
	tp->block_number = 0;
	tp->text[0] = GetPackCount(0x88);	 // pack count
	tp->text[1] = GetPackCount(0x89);	 // pack count
	tp->text[2] = GetPackCount(0x8A);	 // pack count
	tp->text[3] = GetPackCount(0x8B);	 // pack count
	tp->text[4] = GetPackCount(0x8C);	 // pack count
	tp->text[5] = GetPackCount(0x8D);	 // pack count
	tp->text[6] = GetPackCount(0x8E);	 // pack count
	tp->text[7] = GetPackCount(0x8F) + 2; // pack count
	tp->text[8] = seq_num + 1;			  // last sequence number
	tp->text[9] = 0;
	tp->text[10] = 0;
	tp->text[11] = 0;
	fillcrc((uchar*)(tp), sizeof(*tp));
	dataL->AddItem(tp);

	tp = new textpack();
	tp->pack_type = PTI_SIZE;
	tp->track_no = 2;
	tp->seq_number = seq_num;
	tp->block_number = 0;
	tp->text[0] = 0;
	tp->text[1] = 0;
	tp->text[2] = 0;
	tp->text[3] = 0;
	tp->text[4] = fLanguage;
	tp->text[5] = 0;
	tp->text[6] = 0;
	tp->text[7] = 0;
	tp->text[8] = 0;
	tp->text[9] = 0;
	tp->text[10] = 0;
	tp->text[11] = 0;
	fillcrc((uchar*)(tp), sizeof(*tp));
	dataL->AddItem(tp);

	for (int i = 0; i < dataL->CountItems(); i++) {
		Write((void*)dataL->ItemAt(i), sizeof(*((textpack*)dataL->ItemAt(i))));
		delete dataL->ItemAt(i);
	}

	filesize = (seq_num + 1) * sizeof(textpack) + 2;
	hbuf[0] = filesize / 256;
	hbuf[1] = filesize % 256;
	hbuf[2] = 0;
	hbuf[3] = 0;

	Seek(0, SEEK_SET);
	Write((void*)hbuf, 4);
}

void CDTextFile::SetAlbum(const char* text)
{
	list* l = new list();

	l->type = (uchar)PTI_TITLE;
	l->track_no = 0;
	sprintf(l->text, "%s", text);
	stringL->AddItem(l);
}

const char* CDTextFile::GetAlbum()
{
	list* l;

	for (int32 i = 0; i < stringL->CountItems(); i++) {
		l = (list*)stringL->ItemAt(i);
		if ((l->track_no == 0) && (l->type == PTI_TITLE)) {
			return strdup(l->text);
		}
	}
	return NULL;
}

void CDTextFile::SetPerformer(int track, const char* text)
{
	list* l = new list();

	l->type = (uchar)PTI_PERFORMER;
	l->track_no = track;
	sprintf(l->text, "%s", text);
	stringL->AddItem(l);
}

const char* CDTextFile::GetPerformer(int track)
{
	list* l;

	for (int32 i = 0; i < stringL->CountItems(); i++) {
		l = (list*)stringL->ItemAt(i);
		if ((l->track_no == track) && (l->type == PTI_PERFORMER)) {
			return strdup(l->text);
		}
	}
	return NULL;
}

void CDTextFile::SetTrack(int track, const char* text)
{
	list* l = new list();

	l->type = PTI_TITLE;
	l->track_no = track;
	sprintf(l->text, "%s", text);
	stringL->AddItem(l);
}

const char* CDTextFile::GetTrack(int track)
{
	list* l;

	for (int32 i = 0; i < stringL->CountItems(); i++) {
		l = (list*)stringL->ItemAt(i);
		if ((l->track_no == track) && (l->type == PTI_TITLE)) {
			return strdup(l->text);
		}
	}
	return NULL;
}

status_t CDTextFile::read_header()
{
	char header[4];

	Seek(0, SEEK_SET);
	if (Read((void*)header, 4) == 4)
		return B_OK;
	else
		return B_ERROR;
}

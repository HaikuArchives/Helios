#ifndef _MEDIAINFO_H_
#define _MEDIAINFO_H_

#include <Entry.h>
#include <MediaFile.h>
#include <Bitmap.h>

class TrackLength {
public:
	TrackLength(BEntry *);
	~TrackLength();
	
	size_t		GetBytes();
	bigtime_t	GetMicroSeconds() {return duration;};
	
private:
	bigtime_t	duration;
};

/*

class MMediaInfo {
	public:
		MMediaInfo(BEntry file);
		virtual ~MMediaInfo();
		
		media_codec_info *CodecInfo();
		off_t Size();
		BBitmap *GetIcon();

	private:
		BEntry *entry;
		BMediaFile *mfile;
		entry_ref ref;
		BMediaTrack *track;
		char *cname;
		media_codec_info mf;
		BBitmap *bmp;
		bool media;
		bool raw;
};
*/
#endif

#include "MediaInfo.h"
#include <stdlib.h>
#include <math.h>
#include <MediaFile.h>
#include <MediaTrack.h>
#include <Entry.h>
#include <File.h>
#include <Path.h>
#include <stdio.h>
#include <Node.h>
#include <string.h>
#include <NodeInfo.h>

bigtime_t xround(float f) {

	if ((float)(f-(bigtime_t)f)>0.000F) return ((bigtime_t)f)+1; else return (bigtime_t)f;
}

TrackLength::TrackLength(BEntry *entry) {
	entry_ref	ref;
	
	entry->GetRef(&ref);
	entry->Unset();
	delete entry;
	
	BMediaFile *mf=new BMediaFile(&ref);
	
	if (mf->InitCheck()==B_OK)
		duration=mf->TrackAt(0)->Duration();
	else
		duration=0;
}

TrackLength::~TrackLength() {
}

size_t TrackLength::GetBytes() {
	return xround(2352.0F*(float)duration/13333.3333333333333333333F);
}
/*



MMediaInfo::MMediaInfo(BEntry file) {
	entry=new BEntry();
	BPath path;
	media=false;
	raw=false;
	BNode *node=new BNode();
	char ftype[B_MIME_TYPE_LENGTH];




	file.GetPath(&path);
	if (entry->SetTo(path.Path(), true)==B_OK)
		if (entry->InitCheck()==B_OK)
			if (entry->GetRef(&ref)==B_OK)
				{
				node->SetTo(&ref);
				BNodeInfo info(node);				
				if (info.GetType(ftype)!=B_OK) 
					{
					media=false;
					path.Unset();
					delete entry;
					delete node;
					return;
					}
				bmp=new BBitmap(BRect(0,0,15,15),B_CMAP8);
				if (info.GetTrackerIcon(bmp, B_MINI_ICON)!=B_OK) bmp=NULL;
				if (strstr(ftype,"audio/")==NULL)
					{
					media=false;
					path.Unset();
					delete entry;
					delete node;
					return;
					}

				raw=true;
				mfile=new BMediaFile(&ref);
				if(mfile->InitCheck()==B_OK)
					media=true;
				}
path.Unset();
file.Unset();
delete node;
delete entry;
}

BBitmap
*MMediaInfo::GetIcon() {
	return bmp;
}


MMediaInfo::~MMediaInfo() {
	//if (mfile!=NULL) mfile->ReleaseAllTracks();
}


bigtime_t xround(float f) {

	if ((float)(f-(bigtime_t)f)>0.000F) return ((bigtime_t)f)+1; else return (bigtime_t)f;
}


media_codec_info *MMediaInfo::CodecInfo() {
	return &mf;
}


off_t MMediaInfo::Size() {
	off_t dur=0;
	off_t length=0;

	//if (mfile==NULL) return -3;
	if (media)	
	{
	mfile->CountTracks();
	track=mfile->TrackAt(0);
	if (track->InitCheck()!=B_OK)
		entry->GetSize(&length); else
	{
	track->GetCodecInfo(&mf);
	dur=track->Duration();
//	do
//		{
//		track->ReadFrames((void *)buffer, &framesdecoded);
//		length+=framesdecoded*4;
//		} while (framesdecoded>0);
//
	mfile->ReleaseAllTracks();
	length=xround(2352.0*(float)dur/13333.3333333333);
	}
//	printf("DURATION: %lld\n",dur);
//	printf("LENGTH: %lld\n",length);
delete mfile;
} else
if (raw) entry->GetSize(&length); else return -1;
//delete entry;
	return (off_t)length;
}*/



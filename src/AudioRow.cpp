#include "AudioRow.h"
#include "Defines.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


AudioRow::AudioRow(int32 track, const char *text, int32 pause, off_t size, off_t length):BRow(){

	trackFIELD=new BIntegerField(track);
	pregapFIELD=new BStringField(Format(pause));
	nameFIELD=new BStringField(text);
	lengthFIELD=new BStringField(Format(length));
	bytesFIELD=new BSizeField(size);
	indexFIELD=new BStringField("0.00s");
	cdtextFIELD=new BStringField("");
	SetField(trackFIELD, TRACKNUMBER_FIELD);
	SetField(pregapFIELD, PREGAP_FIELD);
	SetField(nameFIELD, TRACKNAME_FIELD);
	SetField(lengthFIELD, LENGTH_FIELD);
	SetField(bytesFIELD, BYTES_FIELD);
	SetField(indexFIELD, INDEX_FIELD);
	SetField(cdtextFIELD, CDTEXT_FIELD);

	indexL=new BList();
	float	*f=new float;
	*f=0;
	indexL->AddItem((void *)f);

	splitindex=0;
	SetSplitted(false);
	SetPregap(pause);
	SetLength(length);
	SetTrackNumber(track);
	fEntry=NULL;
	kListc=1;
}

//AudioRow::AudioRow(int32 track, const char *text, int32 pause, off_t size):BRow() {
//	AudioRow(track, text, 0, size, size*176400);
//}

AudioRow::~AudioRow() {
//	if (indexL) {
//		for (int32 i=0; i<indexL->CountItems(); i++) {
//			delete (float *)indexL->RemoveItem(i);
//		}
//	}
	if (fEntry) {
		fEntry->Unset();
		delete fEntry;
	}
	delete indexL;
}

const char *AudioRow::Format(off_t seconds) {
	BString temp="";
	
	temp << seconds/60 << ":" << (seconds%60<10 ? "0" : "") << seconds%60;
	
	return strdup(temp.String());
}

off_t xyround_off_t(float f) {

	if ((float)(f-(off_t)f)>0.000F) return ((off_t)f)+1; else return (off_t)f;
}

void AudioRow::SetBytes(off_t bytes) {
	bytesFIELD->SetSize(bytes);
}

off_t AudioRow::GetBytes() {
	return static_cast<off_t>((2352.0F*GetLength()));
//	return bytesFIELD->Size();
}

void AudioRow::SetLength(off_t seconds) {
	char	temp[64];
	int min, sec;
	
	min=(int)((float)seconds/(75.0F*60.0F));
	sec=(int)(((float)seconds)/75.0F+0.5F)-(60*min);
	
	if (sec==60) {
		min++;
		sec=0;
	}
	
	sprintf(temp, "%d:%s%d", min,  ((sec<10)?"0":""), sec);
	lengthFIELD->SetString(strdup(temp));
	tracklength=seconds;
}

off_t AudioRow::GetLength() {
	return (endF-startF);
	//return (endT-startT)/1000000;
	return tracklength;
}

void AudioRow::SetPregap(int32 frames) {
	char	temp[12];
	
	sprintf(temp, "%ld:%s%ld", frames/75,(frames%75<10 ? "0" : ""), frames%75);
	pregapFIELD->SetString(temp);
	pregapframes=frames;
}

int32 AudioRow::GetPregap() {
	return pregapframes;
}

void AudioRow::SetFilename(const char *leaf) {
	nameFIELD->SetString(leaf);
}

const char *AudioRow::GetFilename() {
	return nameFIELD->String();
}

bool AudioRow::IsSplitted() {
	return splitted;
}

void AudioRow::SetSplitted(bool splitted) {
	this->splitted=splitted;
}

int32 AudioRow::GetSplitIndex() {
	return splitindex;
}

void AudioRow::SetSplitIndex(int32 index) {
	splitindex=index;
}

bool AudioRow::IsListCorrect() {
//	int32 last;
//	
//	if (kListc==1) {kList[0]=0;return true;}
//	if (kListc<1) return false;
//	last=kList[0];
//	for (int i=1; i<kListc;i++) {
//		if ((last>kList[i]) || (GetLength()*75.0<kList[i])) return false;
//		last=kList[i];
//	}
	return true;
}

void AudioRow::SetIndexList(BList *list) {
//	if (indexL) {
//		for (int32 i=0; i<indexL->CountItems(); i++) {
//			delete (float *)indexL->RemoveItem(i);
//		}
//	}
	indexL->MakeEmpty();
	indexL->AddList(list);
	for (int32 i=1; i<indexL->CountItems(); i++) {
		if (*((float *)indexL->ItemAt(i))==0.0F) {
			delete indexL->RemoveItem(i);
			i--;
		}
	}
	indexFIELD->SetString(GetUserList());
}

BList *AudioRow::GetIndexList() {
	return indexL;
}

const char *AudioRow::GetUserList() {
	char	s[12];
	BString	str="";
	for (int32 i=0; i<indexL->CountItems(); i++) {
		sprintf(s, "%0.2fs, ", ((*( (float *)indexL->ItemAt(i)))/75.0F));
		str<<s;
	}
	str.RemoveLast(", ");
	return strdup(str.String());
}

const char *AudioRow::GetCdrecordReadyList() {
	BString	str="";
	for (int32 i=0; i<indexL->CountItems(); i++) {
		str << ((int)(*((float *)indexL->ItemAt(i))))<< ",";
	}
	str.RemoveLast(",");
	return strdup(str.String());
}

const char *AudioRow::GetCDTitle() {
	return cdtextFIELD->String();
}

void AudioRow::SetCDTitle(const char *title) {
	cdtextFIELD->SetString(title);
}

void AudioRow::SetTrackNumber(int32 track) {
	trackFIELD->SetValue(track);
}

int32 AudioRow::GetTrackNumber() {
	return trackFIELD->Value();
}


void AudioRow::SetStartTime(bigtime_t time) {
	startT=time;
	startF=(int64)(((75.0F*(float)time)/1000000.0F)+0.5F);
}

bigtime_t AudioRow::GetStartTime() {
	return startT;
}

void AudioRow::SetEndTime(bigtime_t time) {
	endT=time;
	endF=(int64)(((75.0F*(float)time)/1000000.0F)+0.5F);
}

bigtime_t AudioRow::GetEndTime() {
	return endT;
}

void AudioRow::SetEntry(BEntry *entry) {
	if (fEntry) {
		fEntry->Unset();
		delete fEntry;
	}
	
	fEntry=new BEntry(*entry);
}

const BEntry *AudioRow::GetEntry() {
	return fEntry;
}

void AudioRow::SetStartFrame(int64 frame) {
	startF=frame;
}

int64 AudioRow::GetStartFrame() {
	return startF;
}

void AudioRow::SetEndFrame(int64 frame) {
	endF=frame;
}

int64 AudioRow::GetEndFrame() {
	return endF;
}






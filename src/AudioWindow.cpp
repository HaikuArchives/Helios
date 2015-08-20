#include <stdio.h>

#include <Entry.h>
#include <MediaFile.h>
#include <String.h>
#include <support/Debug.h>
#include <ChannelSlider.h>

#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str)	gMSHLangMgr->_T(str).String()
#define _TPS(str)	gMSHLangMgr->_T(str)
#else
#include <locale/Locale.h>
#endif

#include "Application1.h"
#include "ConfigFields.h"
#include "Bitmaps.h"
#include "TransportButton.h"
#include "AudioWindow.h"


#define LOWERLINE		69
#define MSG_PERIOD	100000
#define INDEX_ACCEPT_MSG	'aw00'
#define ADD_MSG		'aw01'
#define REMOVE_MSG	'aw02'
#define GOTOINDEX_MSG	'aw03'
#define SPLITHERE_MSG	'aw04'
#define SELECTIONSTART_MSG	'aw05'
#define SELECTIONEND_MSG	'aw06'
#define SELECTIONSTARTTC_MSG	'aw07'
#define SELECTIONENDTC_MSG	'aw08'
#define PREGAPCHANGED_MSG	'aw09'
#define VOLUME_CHANGED_MSG	'aw10'

extern bool project_has_changed;

int AudioWindow::compfunc(const void *a,  const void *b) {

	BString	*aS=new BString(), *bS=new BString();
	float	aF, bF;
	
	aS->SetTo((*((BStringItem **)a))->Text());
	bS->SetTo((*((BStringItem **)b))->Text());
	
	aS->RemoveAll("s");
	bS->RemoveAll("s");
	
	aF=atof(aS->String());
	bF=atof(bS->String());

	delete aS;
	delete bS;
	
	if (aF<bF) return -1; else
	if (aF>bF) return 1; else
	return 0;
}


int32	UpdaterThread(void *data) {
	AudioWindow		*audioW=(AudioWindow *)data;
	BString			str;
	int			last_sec=-1,
				last_rsec=-1;
	bigtime_t		currtime;
	bigtime_t		duration;
	int64			last_frame=-1;
	bool			editable=true;
		

	audioW->Lock();
	duration=audioW->audioAO->Track()->Duration();
	audioW->Unlock();

	while (true) {
		audioW->Lock();
		switch(audioW->status) {
			case 0: {
				audioW->playPPB->SetStopped();
				if (!editable) {
					audioW->frameTC->TextView()->MakeEditable((editable=true));
					audioW->frameTC->TextView()->MakeSelectable(editable);
				}
				break;
			}
			case 1: {
				audioW->playPPB->SetPlaying();
				if (editable) {
					audioW->frameTC->TextView()->MakeEditable((editable=false));
					audioW->frameTC->TextView()->MakeSelectable(editable);
				}
				break;
			}
			case 2: {
				audioW->playPPB->SetPaused();
				if (!editable) {
					audioW->frameTC->TextView()->MakeEditable((editable=true));
					audioW->frameTC->TextView()->MakeSelectable(editable);
				}
				break;
			}
			default: break;
		}
		if (last_sec!=(int)((currtime=audioW->audioAO->Track()->CurrentTime())/1000000)) {
			str="";
			str << currtime/60000000 << ":" << (((currtime/1000000)%60<10) ? "0":"") << (currtime/1000000)%60;
			audioW->timeSV->SetText(str.String());
			last_sec=(int)(currtime/1000000);
		}
		
		if (last_rsec!=(int)((duration-(currtime=audioW->audioAO->Track()->CurrentTime()))/1000000)) {
			str="";
			str << "-" << (duration-currtime)/60000000 << ":" << ((((duration-currtime)/1000000)%60<10) ? "0":"") << ((duration-currtime)/1000000)%60;
			audioW->remainingSV->SetText(str.String());
			last_rsec=(int)((duration-currtime)/1000000);
		}
		
		if (last_frame!=(int64)((audioW->audioAO->Track()->CurrentFrame()))) {
			last_frame=(int64)((audioW->audioAO->Track()->CurrentFrame()));
			str="";
			str << last_frame;
			audioW->frameTC->SetText(str.String());
		}

		if ((audioW->status==1) && (audioW->audioAO->Track()->CurrentFrame()>=audioW->audioAO->Track()->CountFrames()))
			audioW->PostMessage(STOP_MSG);
		audioW->Unlock();
		snooze(100000);
	}
	return 0;
}

AudioWindow::AudioWindow(const char *audiofile, AudioRow *ar)
		: BWindow(BRect(0,0,490, 300), "", B_FLOATING_WINDOW_LOOK, B_MODAL_APP_WINDOW_FEEL, B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_ASYNCHRONOUS_CONTROLS) {
		
	float x1, x2;
	fAudioRow=ar;
	
	
	ResizeTo(540, 150);

	// main BView
	mainV=new BView(Bounds(), "mainV", B_FOLLOW_ALL, B_WILL_DRAW);
	mainV->SetViewColor(216, 216, 216);
	AddChild(mainV);
	
	// BBoxes
	
	playerBOX=new BBox(BRect(5,5,226,100), "playerBOX");
	playerBOX->SetLabel(_T("Audio Player")); // "Audio Player"
	mainV->AddChild(playerBOX);
	indexBOX=new BBox(BRect(231,5,389,100), "indexBOX");
	indexBOX->SetLabel(_T("Indices")); // "Indices"
	mainV->AddChild(indexBOX);
	splitBOX=new BBox(BRect(394,5,535,100), "splitBOX");
	splitBOX->SetLabel(_T("Selection")); // "B:Selection"
	mainV->AddChild(splitBOX);
	cdtextBOX=new BBox(BRect(5,105,389,145), "cdtextBOX");
	cdtextBOX->SetLabel(_T("CD-Text")); // "CD-Text"
	mainV->AddChild(cdtextBOX);
	pauseBOX=new BBox(BRect(394,105,535,145), "pauseBOX");
	pauseBOX->SetLabel(_T("Pregap")); // "Pregap"
	mainV->AddChild(pauseBOX);
	
	volumeS=new BSlider(BRect(183, 18, 220, 89), "volumeS", NULL, new BMessage(VOLUME_CHANGED_MSG), 0, 200);
	volumeS->SetOrientation(B_VERTICAL);
	volumeS->SetPosition(0.5);
	volumeS->SetBarThickness(9);
	volumeS->SetModificationMessage(new BMessage(VOLUME_CHANGED_MSG));
	playerBOX->AddChild(volumeS);
		
	// rewind button
	x1=5;
	x2=x1+kRewindBitmapWidth-1;
	rewindTB=new TransportButton(BRect(x1, LOWERLINE, x2, LOWERLINE+kRewindBitmapHeight-1),
				"aw_rewindTB",
				kSkipBackBitmapBits,
				kPressedSkipBackBitmapBits,
				kDisabledSkipBackBitmapBits,
				new BMessage(REWINDED_MSG),
				new BMessage(STARTREWIND_MSG),
				new BMessage(REWINDING_MSG),
				new BMessage(NOTREWINDING_MSG),
				MSG_PERIOD,
				0,
				0,
				B_FOLLOW_LEFT | B_FOLLOW_TOP);
	playerBOX->AddChild(rewindTB);
	
	// PlayPauseButton
	x1=x2+8;
	x2=x1+kPlayPauseBitmapWidth-1;
	playPPB=new PlayPauseButton(BRect(x1,LOWERLINE,x2, LOWERLINE+kPlayPauseBitmapHeight-1),
				"aw_playPPB",
				kPlayButtonBitmapBits,
				kPressedPlayButtonBitmapBits,
				kDisabledPlayButtonBitmapBits,
				kPlayingPlayButtonBitmapBits,
				kPressedPlayingPlayButtonBitmapBits,
				kPausedPlayButtonBitmapBits,
				kPressedPausedPlayButtonBitmapBits,
				new BMessage(PLAYPAUSE_MSG),
				0,
				0,
				B_FOLLOW_LEFT | B_FOLLOW_TOP);	
	playerBOX->AddChild(playPPB);

	// StopButton
	x1=x2+8;
	x2=x1+kStopBitmapWidth-1;
	stopTB=new TransportButton(BRect(x1, LOWERLINE, x2, LOWERLINE+kStopBitmapHeight-1),
				"aw_stopTB",
				kStopButtonBitmapBits,
				kPressedStopButtonBitmapBits,
				kDisabledStopButtonBitmapBits,
				new BMessage(STOP_MSG),
				NULL,
				NULL,
				NULL,
				0,
				0,
				0,
				B_FOLLOW_LEFT | B_FOLLOW_TOP);
	playerBOX->AddChild(stopTB);

	// forward button
	x1=x2+8;
	x2=x1+kRewindBitmapWidth-1;
	forwardTB=new TransportButton(BRect(x1, LOWERLINE, x2, LOWERLINE+kRewindBitmapHeight-1),
				"aw_forwardTB",
				kSkipForwardBitmapBits,
				kPressedSkipForwardBitmapBits,
				kDisabledSkipForwardBitmapBits,
				new BMessage(FORWARDED_MSG),
				new BMessage(STARTFORWARD_MSG),
				new BMessage(FORWARDING_MSG),
				new BMessage(NOTFORWARDING_MSG),
				MSG_PERIOD,
				0,
				0,
				B_FOLLOW_LEFT | B_FOLLOW_TOP);
	playerBOX->AddChild(forwardTB);

	BEntry		*entry=new BEntry(audiofile, true);
	entry_ref	ref;
	entry->GetRef(&ref);
	audioMF=new BMediaFile(&ref);
//	if (audioMF->GetParameterView()!=NULL) {
//		BView	*pV=audioMF->GetParameterView();
//		BWindow *wW=new BWindow(pV->Bounds(), "", B_TITLED_WINDOW, B_NOT_ZOOMABLE);
//		wW->AddChild(pV);
//		wW->Show();
//	}

	ASSERT_WITH_MESSAGE(audioMF->TrackAt(0), "Media file does not contain any tracks!");
	audioAO=new AudioOutput(audioMF->TrackAt(0), "Helios Audio Player");
	delete entry;

	MoveTo(100,100);
	
	float	l;
	l=(playerBOX->Bounds().Width()-38)/2;
	timeSV=new BStringView(BRect(1*l+8+1,LOWERLINE-24,playerBOX->Bounds().Width()-38, LOWERLINE-7), "aw_timeSV", "0", B_FOLLOW_LEFT | B_FOLLOW_TOP);
	timeSV->SetAlignment(B_ALIGN_RIGHT);
	timeSV->SetFont(be_fixed_font);
	timeSV->SetFontSize(12);
	playerBOX->AddChild(timeSV);
	timeSV->SetViewColor(0,25,0);
	timeSV->SetHighColor(0,255,0);
	timeSV->SetLowColor(0,0,0);

	remainingSV=new BStringView(BRect(8,LOWERLINE-24,l+8-1, LOWERLINE-7), "aw_remainingSV", "0", B_FOLLOW_LEFT | B_FOLLOW_TOP);
	remainingSV->SetAlignment(B_ALIGN_LEFT);
	remainingSV->SetFont(be_fixed_font);
	remainingSV->SetFontSize(12);
	playerBOX->AddChild(remainingSV);
	remainingSV->SetViewColor(0,25,0);
	remainingSV->SetHighColor(0,255,0);
	remainingSV->SetLowColor(0,0,0);



	prevframeTB=new TransportButton(BRect(5, LOWERLINE-30-kRewindBitmapHeight, 5+kRewindBitmapWidth-1, LOWERLINE-30-1),
				"aw_prevframeTB",
				kSkipBackBitmapBits,
				kPressedSkipBackBitmapBits,
				kDisabledSkipBackBitmapBits,
				new BMessage(REWINDEDF_MSG),
				new BMessage(STARTREWINDF_MSG),
				new BMessage(REWINDINGF_MSG),
				new BMessage(NOTREWINDINGF_MSG),
				MSG_PERIOD*2,
				0,
				0,
				B_FOLLOW_LEFT | B_FOLLOW_TOP);
	playerBOX->AddChild(prevframeTB);

	nextframeTB=new TransportButton(BRect(playerBOX->Bounds().Width()-35-kRewindBitmapWidth+1, LOWERLINE-30-kRewindBitmapHeight, playerBOX->Bounds().Width()-35, LOWERLINE-30-1),
				"aw_nextframeTB",
				kSkipForwardBitmapBits,
				kPressedSkipForwardBitmapBits,
				kDisabledSkipForwardBitmapBits,
				new BMessage(FORWARDEDF_MSG),
				new BMessage(STARTFORWARDF_MSG),
				new BMessage(FORWARDINGF_MSG),
				new BMessage(NOTFORWARDINGF_MSG),
				MSG_PERIOD*2,
				0,
				0,
				B_FOLLOW_LEFT | B_FOLLOW_TOP);
	playerBOX->AddChild(nextframeTB);


	frameTC=new BTextControl(BRect(5+kRewindBitmapWidth-3+5,LOWERLINE-30-kRewindBitmapHeight+1,playerBOX->Bounds().Width()-35-kRewindBitmapWidth-5-1, LOWERLINE-30-4),
					"aw_frameTC",
					NULL,
					"0",
					new BMessage(INDEX_ACCEPT_MSG));
	frameTC->SetDivider(0);
	frameTC->SetAlignment(B_ALIGN_CENTER, B_ALIGN_CENTER);
	playerBOX->AddChild(frameTC);
	frameTC->TextView()->SetStylable(true);
	frameTC->TextView()->SetFontAndColor(be_fixed_font);
	frameTC->TextView()->SetFontSize(12);
	for (int i=0; i<255; i++)
		frameTC->TextView()->DisallowChar(i);
		
	frameTC->TextView()->AllowChar(B_ENTER);
	frameTC->TextView()->AllowChar(B_BACKSPACE);
	frameTC->TextView()->AllowChar('.');
	frameTC->TextView()->AllowChar('0');
	frameTC->TextView()->AllowChar('1');
	frameTC->TextView()->AllowChar('2');
	frameTC->TextView()->AllowChar('3');
	frameTC->TextView()->AllowChar('4');
	frameTC->TextView()->AllowChar('5');
	frameTC->TextView()->AllowChar('6');
	frameTC->TextView()->AllowChar('7');
	frameTC->TextView()->AllowChar('8');
	frameTC->TextView()->AllowChar('9');
	// seconds
	frameTC->TextView()->AllowChar('s');
	frameTC->TextView()->AllowChar('S');
	// frames
	frameTC->TextView()->AllowChar('f');
	frameTC->TextView()->AllowChar('F');

	updaterT=spawn_thread(UpdaterThread, "AudioWindow Updater", 5, this);
	status=0;
	resume_thread(updaterT);
	
	
	// indexBOX

	indexLV=new BListView(BRect(8, 8+12, 55, 87), "aw_indexLV", B_MULTIPLE_SELECTION_LIST);
	indexSV=new BScrollView("aw_indexSV", indexLV, B_FOLLOW_LEFT | B_FOLLOW_TOP, 0, false, true);
	indexLV->SetInvocationMessage(new BMessage(GOTOINDEX_MSG));
	indexBOX->AddChild(indexSV);
	
	addB=new BButton(BRect(5,5,5,5), "aw_addB", _T("Add"), new BMessage(ADD_MSG), B_FOLLOW_LEFT | B_FOLLOW_TOP);
	indexBOX->AddChild(addB);
	addB->ResizeToPreferred();	
	addB->MoveTo(indexLV->Frame().right+B_V_SCROLL_BAR_WIDTH+8+5, 6+11);
	removeB=new BButton(BRect(5+addB->Frame().right,5,5,5), "aw_removeB", _T("Delete"), new BMessage(REMOVE_MSG), 
				B_FOLLOW_LEFT | B_FOLLOW_TOP);
	indexBOX->AddChild(removeB);
	removeB->ResizeToPreferred();
	removeB->MoveTo(indexLV->Frame().right+B_V_SCROLL_BAR_WIDTH+8+5, 38+11);
	
	// pauseBOX
	pauseTC=new BTextControl(BRect(5,15, pauseBOX->Bounds().right-7, pauseBOX->Bounds().bottom-5), "pauseTC", 
					_T("Pause"), "0.00s", new BMessage(PREGAPCHANGED_MSG), B_FOLLOW_LEFT | B_FOLLOW_TOP);
	pauseBOX->AddChild(pauseTC);
	pauseTC->SetDivider(pauseTC->StringWidth(pauseTC->Label())+2);
	
	// cdtextBOX
	cdtextTC=new BTextControl(BRect(5,15, cdtextBOX->Bounds().right-7, cdtextBOX->Bounds().bottom-5), "cdtextTC",
					_T("Title:"), // "T___CDTEXTTITLE"
					"", NULL, B_FOLLOW_LEFT | B_FOLLOW_TOP);
	cdtextBOX->AddChild(cdtextTC);
	cdtextTC->SetDivider(cdtextTC->StringWidth(cdtextTC->Label())+2);
	
	// splitBOX

	startTC=new BTextControl(BRect(8,6+14, 78,6+14), "startTC", _T("Start:"), // "B:Start"
					"", new BMessage(SELECTIONSTARTTC_MSG), B_FOLLOW_LEFT | B_FOLLOW_TOP);
	splitBOX->AddChild(startTC);
	startTC->SetDivider(startTC->StringWidth(startTC->Label())+2);

	selectionstartB=new BButton(BRect(0,0,0,0), "aw_selectionstartB", _T("Here"), // "B:Here"
					new BMessage(SELECTIONSTART_MSG));
	splitBOX->AddChild(selectionstartB);
	selectionstartB->ResizeToPreferred();
	selectionstartB->ResizeTo(selectionstartB->StringWidth(selectionstartB->Label())+28, selectionstartB->Bounds().Height());
	selectionstartB->MoveTo(83, 6+11);

	endTC=new BTextControl(BRect(8,38+14, 78,38+14), "endTC", _T("End:"), // "B:End"
					"", new BMessage(SELECTIONENDTC_MSG), B_FOLLOW_LEFT | B_FOLLOW_TOP);
	splitBOX->AddChild(endTC);
	endTC->SetDivider(endTC->StringWidth(endTC->Label())+2);

	selectionendB=new BButton(BRect(0,0,0,0), "aw_selectionendB", _T("Here"), // "B:Here"
					new BMessage(SELECTIONEND_MSG));
	splitBOX->AddChild(selectionendB);
	selectionendB->ResizeToPreferred();
	selectionendB->ResizeTo(selectionendB->StringWidth(selectionendB->Label())+28, selectionendB->Bounds().Height());
	selectionendB->MoveTo(83, 38+11);

//	BChannelSlider	*oc=new BChannelSlider(BRect(x2+5,5,Bounds().right-5,Bounds().Height()-5), "name", "Volume", NULL, B_VERTICAL, 2);
//	mainV->AddChild(oc);
	indexL=new BList();
	
	char pos[32];
	
	sprintf(pos, "%0.2fs", fAudioRow->GetStartFrame()/75.0F);
	startTC->SetText(pos);
	sprintf(pos, "%0.2fs", fAudioRow->GetEndFrame()/75.0F);
	endTC->SetText(pos);
	
	PostMessage(PLAYPAUSE_MSG);
	PostMessage(VOLUME_CHANGED_MSG);
}

AudioWindow::~AudioWindow() {
	kill_thread(updaterT);
	if (audioAO->IsPlaying())
		audioAO->Stop();
	delete audioAO;
	audioMF->ReleaseAllTracks();
	delete audioMF;
	delete indexL;
}

void	AudioWindow::PlayChunk() {
//	int64	frame=audioAO->Track()->CurrentFrame();
//	if (status==2) {
//		audioAO->Play();
//		while (audioAO->Track()->CurrentFrame()<frame+5)
//			snooze(1000);
//		audioAO->Stop();
//		audioAO->SeekToFrame(&frame);
//	}
	int64	time=audioAO->Track()->CurrentTime();
	if (status==2) {
		audioAO->Play();
		while (audioAO->Track()->CurrentTime()<time+70000)
			snooze(1000);
		audioAO->Stop();
		audioAO->SeekToTime(&time);
	}
}

void	AudioWindow::SetCDText(const char *text) {
	cdtextTC->SetText(text);
}

void	AudioWindow::SetPregap(int32 frames) {
	char	s[16];
	sprintf(s, "%0.2fs", (float)frames/75.0F);
	pauseTC->SetText(s);
}

BStringItem	*AudioWindow::FindIndex(float index) {
	char 	s[12];
	sprintf(s, "%0.2fs", index/75.0);
	for (int32 i=0; i<indexLV->CountItems(); i++) {
		if (strcmp(((BStringItem *)indexLV->ItemAt(i))->Text(), s)==0) {
			return (BStringItem *)indexLV->ItemAt(i);
		}
	}
	return NULL;
}

float	*AudioWindow::FindIndex(BStringItem *item) {
	char 	s[12];

	for (int32 i=0; i<indexL->CountItems(); i++) {
		sprintf(s, "%0.2fs", (*((float *)indexL->ItemAt(i)))/75.0);
		if (strcmp(item->Text(), s)==0) {
			return (float *)indexL->ItemAt(i);
		}
	}
	return NULL;
}

void	AudioWindow::AddIndex(float index) {
	char	s[12];
	float	*f=new float;
	BStringItem	*si;
	
	*f=index;
	if ((si=FindIndex(index))==NULL) {
		sprintf(s, "%0.2fs", index/75.0);
		indexLV->AddItem(new BStringItem(s));
		indexL->AddItem(f);
	} else {
		indexLV->DeselectAll();
		indexLV->Select(indexLV->IndexOf(si));
	}
	
	indexLV->SortItems((int (*)(const void *, const void *))compfunc);
}

void	AudioWindow::RemoveIndex(float index) {
	char	s[12];
	float	*f;
	BStringItem	*si;
	
	sprintf(s, "%0.2fs", 0.0F);
	if ((si=FindIndex(index))!=NULL) {
		if (
		//(strcmp(si->Text(), s)!=0) ||
		 (indexLV->IndexOf(si)>0)) {
			if ((f=FindIndex(si))!=NULL) {
				if (indexL->RemoveItem((void *)f)) {
					indexLV->RemoveItem(si);
					delete f;
				}
			}
		}
	}
}

void	AudioWindow::GotoIndex(float index) {
	bigtime_t	time=(bigtime_t)(1000000.0F*index/75.0F);
	audioAO->SeekToTime(&time);
}


BList	*AudioWindow::IndexList() {
	return indexL;
}

void	AudioWindow::SetIndexList(BList *list) {
	if (indexL) {
		for (int32 i=0; i<indexL->CountItems(); i++) {
			delete (float *)indexL->RemoveItem(i);
		}
	}
	indexL->AddList(list);
	for (int32 i=1; i<indexL->CountItems(); i++) {
		if (*((float *)indexL->ItemAt(i))==0.0F) {
			delete indexL->RemoveItem(i);
			i--;
		}
	}
	ShowList();
}

void	AudioWindow::ShowList() {
	char	s[12];
	for (int32 i=0; i<indexL->CountItems(); i++) {
		sprintf(s, "%0.2fs", (*((float *)indexL->ItemAt(i)))/75.0);
		indexLV->AddItem(new BStringItem(s));
	}
	indexLV->ItemAt(0)->SetEnabled(false);
	indexLV->SortItems((int (*)(const void *, const void *))compfunc);
}

void	AudioWindow::MessageReceived(BMessage *msg) {
	switch(msg->what) {
		case PLAYPAUSE_MSG: {
			if (audioAO->IsPlaying()) {
				audioAO->Stop();
				playPPB->SetPaused();
				status=2;
			} else {
				audioAO->Play();
				playPPB->SetPlaying();
				status=1;
			}
			break;
		}

		case STOP_MSG: {
			audioAO->Stop();
			bigtime_t	t=0;
			audioAO->SeekToTime(&t);
			playPPB->SetStopped();
			status=0;
			break;
		}
		
		case STARTREWIND_MSG: {
			// dont break!
		}
		case REWINDING_MSG: {
			bigtime_t	t=audioAO->Track()->CurrentTime();
			if (t>1000000)
				t-=1000000;
			else
				t=0;
			audioAO->SeekToTime(&t);
			PlayChunk();
			break;
		}
		
		case STARTFORWARD_MSG: {
			// dont break!
		}
		case FORWARDING_MSG: {
			bigtime_t	t=audioAO->Track()->CurrentTime();
			if (t<audioAO->Track()->Duration()-1000000)
				t+=1000000;
			else {
				audioAO->Stop();
				bigtime_t	t=0;
				audioAO->SeekToTime(&t);
				playPPB->SetStopped();
				break;
			}
			audioAO->SeekToTime(&t);
			PlayChunk();
			break;
		}
		
		case STARTREWINDF_MSG: {
			// dont break!
			delta=1;
			counter=0;
		}
		case REWINDINGF_MSG: {
			int64	t=audioAO->Track()->CurrentFrame();
			if (t>=delta)
				t-=(int64)delta;
			else
				t=0;
			audioAO->SeekToFrame(&t);
			PlayChunk();
			
			counter++;
			if (counter>0) {
				delta+=2;
				counter=0;
			}
			break;
		}
		
		case STARTFORWARDF_MSG: {
			// dont break!
			delta=1;
			counter=0;
		}
		case FORWARDINGF_MSG: {
			int64	t=audioAO->Track()->CurrentFrame();
			if (t<audioAO->Track()->CountFrames())
				t+=(int64)delta;
			else {
				audioAO->Stop();
				t=0;
				audioAO->SeekToFrame(&t);
				playPPB->SetStopped();
				break;
			}
			audioAO->SeekToFrame(&t);
			PlayChunk();

			counter++;
			if (counter>0) {
				delta+=2;
				counter=0;
			}
			break;
		}
		
		case INDEX_ACCEPT_MSG: {
			BString	str=frameTC->Text();
			
			if (str.FindFirst("s")!=B_ERROR) {
				str.RemoveFirst("s");
				bigtime_t	time=(bigtime_t)(atof(str.String())*1000000.0F);
				audioAO->SeekToTime(&time);
			} else
			if (str.FindFirst("f")!=B_ERROR) {
				str.RemoveFirst("f");
				bigtime_t	time=(bigtime_t)atol(str.String());
				time*=1000000;
				time/=75;
				audioAO->SeekToTime(&time);
			} else {
				int64	frame=(int64)atol(frameTC->Text());
				
				// range check
				if (frame<0)
					frame=0;
				if (frame>audioAO->Track()->Duration()*75)
					frame=audioAO->Track()->Duration()*75;
					
				audioAO->SeekToFrame(&frame);
			}
			project_has_changed=true;
			break;
		}

		case ADD_MSG: {
			bigtime_t	time=audioAO->Track()->CurrentTime();
			AddIndex((75.0F*(float)time)/1000000.0F);
			project_has_changed=true;
			break;
		}
		
		case REMOVE_MSG: {
			int32	s;
			
			while ((s=indexLV->CurrentSelection(0))>=0)
				RemoveIndex(*FindIndex((BStringItem *)(indexLV->ItemAt(s))));
			project_has_changed=true;
			break;
		}

		case GOTOINDEX_MSG: {
			int32	index;
			msg->FindInt32("index", &index);
			GotoIndex(*FindIndex((BStringItem *)(indexLV->ItemAt(index))));
			break;
		}

		case SPLITHERE_MSG: {
			break;
		}
		
		case SELECTIONSTART_MSG: {
			char pos[32];
			
			sprintf(pos, "%0.2fs", audioAO->Track()->CurrentTime()/1000000.0F);
			startTC->SetText(pos);
			fAudioRow->SetStartFrame((int64)(75.0F*(float)audioAO->Track()->CurrentTime()/1000000.0F));
			project_has_changed=true;
			break;
		}
		
		case SELECTIONEND_MSG: {
			char pos[32];
			
			sprintf(pos, "%0.2fs", audioAO->Track()->CurrentTime()/1000000.0F);
			endTC->SetText(pos);
			fAudioRow->SetEndFrame((int64)(75.0F*(float)audioAO->Track()->CurrentTime()/1000000.0F));
			project_has_changed=true;
			break;
		}

		case SELECTIONSTARTTC_MSG: {
			int64 frame;
			
			BString	str=startTC->Text();
			if (str.FindFirst("s")!=B_ERROR) {
				str.RemoveFirst("s");
				frame=(int64)(atof(str.String())*75.0F);
			} else
			if (str.FindFirst("f")!=B_ERROR) {
				str.RemoveFirst("f");
				frame=atoll(str.String());
			} else {
				frame=(int64)(atof(str.String())*75.0F);
			}
			if (frame<0)
				frame=0;
	
			fAudioRow->SetStartFrame(frame);
			
			char pos[32];
			
			sprintf(pos, "%0.2fs", (frame/75.0F));
			startTC->SetText(pos);
			project_has_changed=true;
			break;
		}
		
		case SELECTIONENDTC_MSG: {
			int64 frame;
			
			BString	str=endTC->Text();
			if (str.FindFirst("s")!=B_ERROR) {
				str.RemoveFirst("s");
				frame=(int64)(atof(str.String())*75.0F);
			} else
			if (str.FindFirst("f")!=B_ERROR) {
				str.RemoveFirst("f");
				frame=atoll(str.String());
			} else {
				frame=(int64)(atof(str.String())*75.0F);
			}

			if (frame<0)
				frame=(int64)((75.0F*(float)(audioAO->Track()->Duration())/1000000.0F)-1.0F);
	
			fAudioRow->SetEndFrame(frame);
			
			char pos[32];
			
			sprintf(pos, "%0.2fs", (frame/75.0F));
			endTC->SetText(pos);
			project_has_changed=true;
			break;
		}
		
		case PREGAPCHANGED_MSG: {
			int64 frame;
			
			BString	str=pauseTC->Text();
			if (str.FindFirst("s")!=B_ERROR) {
				str.RemoveFirst("s");
				frame=(int64)(atof(str.String())*75.0F);
			} else
			if (str.FindFirst("f")!=B_ERROR) {
				str.RemoveFirst("f");
				frame=atoll(str.String());
			} else {
				frame=(int64)(atof(str.String())*75.0F);
			}
			
			if (frame<0)
				frame=0;
	
			fAudioRow->SetPregap(frame);
			
			char pos[32];
			
			sprintf(pos, "%0.2fs", (frame/75.0F));
			pauseTC->SetText(pos);
			project_has_changed=true;

			break;
		}
		
		case VOLUME_CHANGED_MSG: {
			audioAO->SetVolume(volumeS->Position()*volumeS->Position()*volumeS->Position());
			break;
		}

		default: {
			BWindow::MessageReceived(msg);
			break;
		}
	}
}




void	AudioWindow::SetAudioRow(AudioRow *row) {
	fAudioRow=row;
}

bool	AudioWindow::QuitRequested() {
	fAudioRow->SetIndexList(indexL);
	fAudioRow->SetCDTitle(cdtextTC->Text());
	fAudioRow->SetLength((fAudioRow->GetEndFrame()-fAudioRow->GetStartFrame()));
	fAudioRow->SetBytes(fAudioRow->GetBytes());
	// pregap
	int32 frames;
	BString	str=pauseTC->Text();
	if (str.FindFirst("s")!=B_ERROR) {
		str.RemoveFirst("s");
		frames=(int32)(atof(str.String())*75.0F);
	} else
	if (str.FindFirst("f")!=B_ERROR) {
		str.RemoveFirst("f");
		frames=atol(str.String());
	} else
		frames=(int32)(atof(str.String())*75.0F);
	if (frames<0) frames=0;
	if (frames>4800) frames=4800;
	fAudioRow->SetPregap(frames);
	
	// save window position
	((Application1 *)be_app)->CM->SetRect(AUDIOWINDOW_RECT, Frame());
	be_app->PostMessage(AUDIOWINDOW_RETURN_MSG);
	Quit();
	return false;
}


















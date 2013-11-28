#ifndef _AUDIOWINDOW_H_
#define _AUDIOWINDOW_H_

#include <InterfaceKit.h>
#include "TransportButton.h"
#include "AudioOutput.h"
#include <MediaFile.h>
#include <MediaTrack.h>
#include "AudioRow.h"

#define PLAYPAUSE_MSG		'AW00'
#define STOP_MSG			'AW01'

#define REWINDED_MSG		'AW02'
#define STARTREWIND_MSG	'AW03'
#define REWINDING_MSG		'AW04'
#define NOTREWINDING_MSG	'AW05'
#define FORWARDED_MSG		'AW06'
#define STARTFORWARD_MSG	'AW07'
#define FORWARDING_MSG	'AW08'
#define NOTFORWARDING_MSG	'AW09'
// frame
#define REWINDEDF_MSG		'AW10'
#define STARTREWINDF_MSG	'AW11'
#define REWINDINGF_MSG		'AW12'
#define NOTREWINDINGF_MSG	'AW13'
#define FORWARDEDF_MSG	'AW14'
#define STARTFORWARDF_MSG	'AW15'
#define FORWARDINGF_MSG	'AW16'
#define NOTFORWARDINGF_MSG	'AW17'

#define AUDIOWINDOW_RETURN_MSG	'AW18'



class AudioWindow : public BWindow {
	public:
		AudioWindow(const char *audiofile, AudioRow *);
		~AudioWindow();

		BList	*IndexList();
		void	SetIndexList(BList *);
		void	SetAudioRow(AudioRow *);
		void	SetCDText(const char *text);
		void	SetPregap(int32 frames);

	
	private:
		friend int32 UpdaterThread(void *data);
	
		void			ShowList();
		static int		compfunc(const void *a,  const void *b);
		void			AddIndex(float index);
		BStringItem		*FindIndex(float index);
		float			*FindIndex(BStringItem *item);
		void			GotoIndex(float index);
		void			RemoveIndex(float index);
		virtual void	MessageReceived(BMessage *);
		virtual bool	QuitRequested();
		void			PlayChunk();
	
		BTextControl		*pauseTC,
					*cdtextTC,
					*frameTC,
					*startTC,
					*endTC;
		BButton		*addB,
					*removeB;
		BView			*mainV;
		PlayPauseButton	*playPPB;
		TransportButton	*stopTB,
					*rewindTB,
					*forwardTB,
					*nextframeTB,
					*prevframeTB;
		AudioOutput		*audioAO;
		BMediaFile		*audioMF;
		thread_id		updaterT;
		int			status;
		BStringView		*timeSV,
					*remainingSV,
					*framesSV;
					
		int64			counter;
		bigtime_t		period;
		float			delta;
		int64			currentFrame;
		
		BListView		*indexLV;
		BScrollView		*indexSV;
		BList			*indexL;
		AudioRow		*fAudioRow;
		
		BBox			*playerBOX,
					*indexBOX,
					*splitBOX,
					*cdtextBOX,
					*pauseBOX;
					
		BButton		*selectionstartB,
					*selectionendB,
					*mergedownB,
					*mergeupB;
		BSlider		*volumeS;
};


#endif


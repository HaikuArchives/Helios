#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <Application.h>
#include <Button.h>
#include <String.h>
#include <Window.h>
#include <Directory.h>
#include <Entry.h>

#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str)	gMSHLangMgr->_T(str).String()
#define _TPS(str)	gMSHLangMgr->_T(str)
#else
#include <locale/Locale.h>
#endif

#include "Application1.h"
#include "WarningBitmap.h"
#include "InfoBox.h"

typedef struct ThreadData{
	bool	interruptFlag;
	bool	updateFlag;
	void	*parent;
	int	seconds;
};
ThreadData	d;
sem_id		exit_thread_sem;

InfoBox::InfoBox()
 : BBox(BRect(317, 90, 589, 169), "box", B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW , B_FANCY_BORDER) {
 
 //	levelSB=new BStatusBar(BRect(7, 45, 221, 80), "ib_levelSB");
  	levelSB=new BStatusBar(BRect(7, 37, 201, 73), "ib_levelSB");
	AddChild(levelSB);
 	levelSB->SetText(_T("CD size:")); // "InfoBox:CD Size:"
 	levelSB->SetTrailingText("?");
 	levelSB->SetFontSize(10);
 
	stringview3 = new BStringView(BRect(8, 9, 134, 21), "StringView", _T("Total CD size:"), // "TTOTALCDSIZE"
				B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW);
	stringview3->SetFontSize(10);
//	AddChild(stringview3);

	stringview9 = new BStringView(BRect(8, 23, 134, 35), "StringView", _T("Total CD duration:"), // "TCDLENGTH"
				B_FOLLOW_LEFT| B_FOLLOW_TOP, B_WILL_DRAW);
	stringview9->SetFontSize(10);
//	AddChild(stringview9);

	stringview4 = new BStringView(BRect(8, 37, 134, 49), "StringView", _T("Estimated time:"), // "TESTIMATEDTIME"
				B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW);
	stringview4->SetFontSize(10);
//	AddChild(stringview4);

	filesSV=new BStringView(BRect(7,5,134,20),"StringView",_T("Number of files:")); // "TFILECOUNT"
	filesSV->SetFontSize(10);
	AddChild(filesSV);

	foldersSV=new BStringView(BRect(7,21,134,36),"StringView",_T("Number of folders:")); // "TFOLDERCOUNT"
	foldersSV->SetFontSize(10);
	AddChild(foldersSV);

	stringview5 = new BStringView(BRect(135, 9, 264, 21), "StringView", "0.00MB", B_FOLLOW_RIGHT | B_FOLLOW_TOP, B_WILL_DRAW);
	stringview5->SetAlignment(B_ALIGN_RIGHT);
	stringview5->SetFontSize(10);
//	AddChild(stringview5);

	stringview10 = new BStringView(BRect(135, 23, 264, 35), "StringView", "0m 00s", B_FOLLOW_RIGHT | B_FOLLOW_TOP, B_WILL_DRAW);
	stringview10->SetAlignment(B_ALIGN_RIGHT);
	stringview10->SetFontSize(10);
//	AddChild(stringview10);

	stringview6 = new BStringView(BRect(135, 37, 264, 49), "StringView", "0m 00s", B_FOLLOW_RIGHT | B_FOLLOW_TOP, B_WILL_DRAW);
	stringview6->SetAlignment(B_ALIGN_RIGHT);
	stringview6->SetFontSize(10);
//	AddChild(stringview6);

	filecounterSV = new BStringView(BRect(135, 5, 201, 20), "StringView", "0", B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW);
	filecounterSV->SetAlignment(B_ALIGN_RIGHT);
	filecounterSV->SetFontSize(10);
	AddChild(filecounterSV);

	foldercounterSV = new BStringView(BRect(135, 21, 201, 36), "StringView", "0", B_FOLLOW_LEFT | B_FOLLOW_TOP, B_WILL_DRAW);
	foldercounterSV->SetAlignment(B_ALIGN_RIGHT);
	foldercounterSV->SetFontSize(10);
	AddChild(foldercounterSV);

	//SetLabel(_T("Info")); // "TINFO"
	// UPDATE BUTTON
//	button3=new BButton(BRect(204,48,264,74),"Button3",TUPDATE,new BMessage(UPDATEINFO_MSG),B_FOLLOW_RIGHT | B_FOLLOW_TOP,B_WILL_DRAW);
//	button3->SetTarget(be_app_messenger);
//	button3->SetFontSize(10);
//	AddChild(button3);
	update_Thread=-1;
	cd_size=0;
	exit_thread_sem=create_sem(0, "exit_thread_sem");
	ut_running=false;
	warningBM=NULL;


	
}

InfoBox::~InfoBox(void) {
	delete_sem(exit_thread_sem);
	delete warningBM;
}

void	InfoBox::AttachedToWindow() {
	if (warningBM==NULL)
		warningBM=new BBitmap(BRect(0, 0, kDataWidth-1, kDataHeight-1), B_RGBA32, false, true);
	
	memcpy(warningBM->Bits(), kDataBits, sizeof(kDataBits));
	
 	levelSB->SetBarHeight(20.0);
}

void InfoBox::SetBarColor(rgb_color color) {
	levelSB->SetBarColor(color);
	levelSB->Invalidate();
}

void InfoBox::DrawWarningBitmap() {
	if ((float)cd_size>(float)((levelSB->MaxValue())*(2048.0F))) {
		SetDrawingMode(B_OP_COPY);
		SetHighColor(ViewColor());
		FillRect(BRect(205, 52, 205+kDataWidth, 52+kDataHeight));
		SetDrawingMode(B_OP_ALPHA);
		DrawBitmap(warningBM, BPoint(205, 52));
	} else {
		SetDrawingMode(B_OP_COPY);
		SetHighColor(ViewColor());
		FillRect(BRect(205, 52, 205+kDataWidth, 52+kDataHeight));
		SetHighColor(0,0,0);
	}
}

void InfoBox::Draw(BRect updateRect) {
	BBox::Draw(updateRect);
	DrawWarningBitmap();
}


void InfoBox::UpdateSizes() {
	float	display_size=0;	// Bytes
	float	display_time=0;	// Seconds
	char	bytes[21];		// ex: "kB"
	char	seconds[21];		// ex: "73m12.2s"
	char	temp[12];

	display_size=GetCDSize();
	display_time=(2352.0F*display_size/2048.0F)/176400.0F;
	
	// calculate playing time
	int min=(int)(display_time/60.0F);
	float sec=display_time-(min*60.0);
	sprintf(seconds, "%dm%s%0.1fs", min, (sec<10?"0":""), sec);	
	
	// calculate Bytes/kBytes/...
	sprintf(bytes, "B");	
	if (display_size>1024) {
		display_size/=1024.0F;
		sprintf(bytes, "kB");
	}
	if (display_size>1024) {
		display_size/=1024.0F;
		sprintf(bytes, "MB");
	}
	if (display_size>1024) {
		display_size/=1024.0F;
		sprintf(bytes, "GB");
	}
	if (display_size>1024) {
		display_size/=1024.0F;
		sprintf(bytes, "TB");
	}
	if (display_size>1024) {
		display_size/=1024.0F;
		sprintf(bytes, "EB");
	}
	sprintf(bytes, "%0.1f%s", display_size, strdup(bytes));

	Window()->Lock();
	switch(((Application1 *)be_app)->window1->view1->CDType()) {
		case DVDVIDEO_INDEX: {
			levelSB->SetMaxValue(2146500.0F);	// 477 minutes/2146500 sectors
			break;
		}
		default: {
			levelSB->SetMaxValue( (((Application1 *)be_app)->heliosCV->IsOverburning()?405000.0F:360000.0F));
			break;
		}
	}


	levelSB->Invalidate();
	levelSB->Update(-levelSB->CurrentValue()); 	// reset bar
	levelSB->Update((float)cd_size/2048.0F);

	levelSB->SetTrailingText(seconds);

	switch(((Application1 *)be_app)->window1->view1->CDType()) {
		case DATACD_INDEX: {
			levelSB->SetTrailingText(bytes);
			break;
		}
		case AUDIOCD_INDEX: {
			levelSB->SetTrailingText(seconds);
			break;
		}
		case CDEXTRA_INDEX: {
			levelSB->SetTrailingText(seconds);
			break;
		}
		case BOOTABLECD_INDEX: {
			levelSB->SetTrailingText(bytes);
			break;
		}
		case DVDVIDEO_INDEX: {
			levelSB->SetTrailingText(bytes);
			break;
		}
	}

	DrawWarningBitmap();

	folders=0;
	files=0;
	GetFolder(BDirectory(((Application1 *)be_app)->base_folder.String()));
	sprintf(temp,"%d",files);
	filecounterSV->SetText(temp);
	sprintf(temp,"%d",folders);
	foldercounterSV->SetText(temp);
	Window()->Unlock();
	return;


/*

	BString 	str="";
	char		str2[32];
	char 		*s="";
	char		temp[10];
	size_t 		u, disp_size;
	bool		gigabyte=false;

	Window()->Lock();
	//levelSB->SetBarColor((rgb_color){50, 150, 255});

//	stringview5->SetHighColor(0,0,0,255);
//	stringview10->SetHighColor(0,0,0,255);

	float r=(size_t)(((size_t)cd_size-((size_t)((float)cd_size/1048576.0F))*1048576.0F)/10485.76F);
	if (r<10) s="0"; else s="";
	
	//
	// TODO: ADD DVD SUPPORT!!!
	//
	if (((Application1 *)be_app)->window1->view1->CDType()==DVDVIDEO_INDEX)
		levelSB->SetMaxValue( 477.0F*60.0F);
	else
		levelSB->SetMaxValue( (((Application1 *)be_app)->heliosCV->IsOverburning()?91.0F:80.0F)*60.0F);
	levelSB->Update(-levelSB->CurrentValue());
	levelSB->Update((double)cd_size/153600.0F);

	DrawWarningBitmap();
	disp_size=cd_size;
	if ((disp_size/1048576)>1024) {
		disp_size/=1024;
		gigabyte=true;
	}

	float Size=disp_size;
	
	
	if ((((Application1 *)be_app)->window1->view1->CDType()==AUDIOCD_INDEX) ||
		(((Application1 *)be_app)->window1->view1->CDType()==CDEXTRA_INDEX)) {
		int min;
		float sec;
		
		min=(int)((float)disp_size/(153600.0F*60.0F));
		sec=((((float)disp_size)/(153600.0F))-((float)min*60.0F));
		//frm=(int)((((((float)cd_size)/(153600.0F))-(((float)min)*60.0F))-(((float)sec)))*75.0F+0.5F   );
		sprintf(str2, "%2.2d:%s%2.2f", min, ((sec<10.0F)?"0":""), sec);
		//str <<  min << ":" << (sec<10?"0":"") << sec << "." << (frm<10?"0":"") << frm;
	} else
		sprintf(str2, "%2.2f%s", (float)(Size/1048576), (gigabyte ? "GB" : "MB"));
		//str << (size_t)(cd_size/1048576) << "." << s << r << (gigabyte ? "GB" : "MB");
//	stringview5->SetText(str.String());
	levelSB->SetTrailingText(str2);
	

	r=(1.1484375F*(float)disp_size);

	if ((size_t)(r/176400)>4440) {	// grösser als 74m
//		levelSB->SetBarColor((rgb_color){180, 100, 120});
//		stringview5->SetHighColor(100,0,0,255);
//		stringview10->SetHighColor(100,0,0,255);
	}
	if ((size_t)(r/176400)>4800) {	// grösser als 80m
//		levelSB->SetBarColor((rgb_color){230, 60, 90});
//		stringview5->SetHighColor(150,0,0,255);
//		stringview10->SetHighColor(150,0,0,255);
	}
	if ((size_t)(r/176400)>5400) {	// grösser als 91m
//		levelSB->SetBarColor((rgb_color){255, 30, 60});
//		stringview5->SetHighColor(200,0,0,255);
//		stringview10->SetHighColor(200,0,0,255);
	}
	
	r=(float)(r/(176400.0F*60.0F));
	u=(size_t)((2352*disp_size/(2048*176400))-(r*60));
	if (u<10) s="0"; else s="";
	str="";
	str << (size_t)r << "m " << s << u << "s";
	stringview10->SetText(str.String());




	off_t size2=disp_size;
	if (((Application1 *)be_app)->heliosCV->IsFixating()) size2+=13824000;
	r=(2352*(size_t)size2/(2048*176400*(size_t)(((Application1 *)be_app)->heliosCV->GetWriterSpeed())));
	str="";
	r+=10; // 10 Sekunden Wartezeit von cdrecord
	r+=10; // und 10 sekunden wartezeit für spinup/spindown
	r+=10; // und dann nochmal 10 sekunden, für alle fälle...
	u=r-60*(size_t)(r/60);
	s="";
	
	if (u<10) s="0"; else s="";
	str << (off_t)(r/60) << "m " << s << (off_t)u << "s";
	stringview6->SetText(str.String());

	folders=0;
	files=0;
	GetFolder(BDirectory(((Application1 *)be_app)->base_folder.String()));
	sprintf(temp,"%d",files);
	filecounterSV->SetText(temp);
	sprintf(temp,"%d",folders);
	foldercounterSV->SetText(temp);

	Window()->Unlock();*/

}


void InfoBox::Refresh() {
	if (ut_running) {
		if (update_Thread>0) {
			d.interruptFlag=true;
			d.updateFlag=true;
		}
	} else {
		((Application1 *)be_app)->GetImageSize();
		UpdateSizes();
	}
}

void InfoBox::SpawnUpdateThread() {
	if (!ut_running) {
		d.interruptFlag=false;
		d.updateFlag=false;
		d.parent=this;
		d.seconds=0;
		update_Thread=spawn_thread(UpdateThread, UPDATETHREADNAME, UPDATETHREAD_PRIORITY, (void *)(&d));
	}
}


void InfoBox::ExitUpdateThread() {
	if (suspend_thread(update_Thread)!=B_BAD_THREAD_ID) {
		//send_data(update_Thread, 'exit', NULL, 0);
		kill_thread(update_Thread);
		//acquire_sem(exit_thread_sem);
	}
	ut_running=false;
}

void InfoBox::SetUpdateFrequency(int32 seconds) {
	if (ut_running) {
		if (seconds>0) {
			d.seconds=seconds;
			//d.interruptFlag=true;
		}
	}
}

void InfoBox::SetCDSize(size_t size) {
	cd_size=size;
	//Refresh();
}

size_t InfoBox::GetCDSize() {
	return cd_size;
}

void InfoBox::StopUpdating() {
	if (ut_running) {
		suspend_thread(update_Thread);
		ut_running=false;
	}
}

void InfoBox::StartUpdating() {
	if (!ut_running) {
		resume_thread(update_Thread);
		ut_running=true;
	}
}

void InfoBox::GetFolder(BDirectory dir) {

	int32 c=dir.CountEntries();
	BEntry entry;
	
	if (c>0)
	for (int32 i=0; i<c; i++) {
		dir.GetNextEntry(&entry, true);
		if (entry.IsDirectory()) {
			folders++;
			GetFolder(BDirectory(&entry));
		}
		else
			files++;
	}
}


int32 InfoBox::UpdateThread(void *data) {

	int32		code;
	ThreadData	*dt=(ThreadData *)data;
	thread_id	sender;
	int32		seconds=0;
	int32		i;
	int32		buffer;	// not a real data buffer. contains the command code, only.
				// (4 bytes long)
				

	
	while (true) {

		// calculate sizes and display them
		if (dt->seconds>0) {
			((Application1 *)be_app)->GetImageSize();
			((InfoBox *)dt->parent)->UpdateSizes();
		}

		// wait...
		for (i=0; i<dt->seconds*4; i++) {
			snooze(250000);
			if (dt->interruptFlag)
				i=dt->seconds*4+1;
		}
		
		snooze(30000);
		dt->interruptFlag=false;
		if (has_data(find_thread(NULL))) {
			code=receive_data(&sender, &buffer, 4);
			switch(code) {
				case 'exit':{
					//release_sem(exit_thread_sem);
					return 0;
				}
				
				case 'leng': {
					seconds=buffer;
					break;
				}

				default: {
					break;
				}
			} // switch
		}
		
	} // while
	return 0;
}


/*
		window1->Update_sizes();
		directories=0;
		files=0;
		GetFolder(BDirectory(base_folder.String()));
		sprintf(temp,"%ld",files);
		window1->view1->box2->filecounterSV->SetText(temp);
		sprintf(temp,"%ld",directories);
		window1->view1->box2->foldercounterSV->SetText(temp);










*/

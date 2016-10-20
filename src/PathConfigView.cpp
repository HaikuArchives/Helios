#include <stdio.h>

#include <Directory.h>

#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str) gMSHLangMgr->_T(str).String()
#define _TPS(str) gMSHLangMgr->_T(str)
#else
#include <locale/Locale.h>
#endif

#include "PathConfigView.h"

#define PROJECTPATH_CHANGED 'PCV0'
#define TEMPPATH_CHANGED 'PCV1'

PathConfigView::PathConfigView(BRect frame) : ConfigView(frame)
{
	// TEXTCONTROLS
	projectpathTC = new BrowseField(
		BRect(15, 5, frame.Width() - 15, 30),
		"pcv_projectpathTC"); //,_T("TPROJECTPATH"), "", new BMessage(PROJECTPATH_CHANGED));
	projectpathTC->GetTextControl()->SetDivider(projectpathTC->StringWidth(_T("Project path:")) +
												5);							 // "TPROJECTPATH"
	projectpathTC->GetButton()->SetLabel(_T("Browse"));						 // "TBROWSE"
	projectpathTC->Init(_T("Select"), NULL, B_FILE_NODE | B_DIRECTORY_NODE); // "C_SELECT"
	projectpathTC->GetTextControl()->SetLabel(_T("Project path:"));			 // "TPROJECTPATH"
	projectpathTC->GetTextControl()->TextView()->SetMaxBytes(8192);
	AddChild(projectpathTC);

	temppathTC =
		new BrowseField(BRect(15, 35, frame.Width() - 15, 60),
						"pcv_temppathTC"); //,_T("TTEMPPATH"), "", new BMessage(TEMPPATH_CHANGED));
	temppathTC->GetTextControl()->SetDivider(temppathTC->StringWidth(_T("Temp. Path:")) +
											 5);	 // "TTEMPPATH"
	temppathTC->GetButton()->SetLabel(_T("Browse")); // "TBROWSE"
	temppathTC->Init(_T("Select"), NULL, B_FILE_NODE | B_DIRECTORY_NODE);
	temppathTC->GetTextControl()->SetLabel(_T("Temp. Path:")); // "TTEMPPATH"
	temppathTC->GetTextControl()->TextView()->SetMaxBytes(8192);
	AddChild(temppathTC);

	imagenameTC = new BrowseField(BRect(15, 65, frame.Width() - 15, 90),
								  "pcv_imagenameTC"); //,_T("TDEFAULTIMAGENAME"), "",NULL);
	imagenameTC->GetTextControl()->SetDivider(
		imagenameTC->StringWidth(_T("Default image file name:")) + 5); // "TDEFAULTIMAGENAME"
	imagenameTC->GetButton()->SetLabel(_T("Browse"));
	imagenameTC->GetButton()->Hide();
	imagenameTC->Init(_T("Select"), NULL, B_FILE_NODE);
	imagenameTC->GetTextControl()->SetLabel(_T("Default image file name:")); // "TDEFAULTIMAGENAME"
	imagenameTC->GetTextControl()->TextView()->SetMaxBytes(8192);
	AddChild(imagenameTC);

	mountpointTC = new BrowseField(BRect(15, 95, frame.Width() - 15, 120),
								   "pcv_mountpointTC"); //,_T("TDEFAULTMOUNTPOINT"), "",NULL);
	mountpointTC->GetTextControl()->SetDivider(
		mountpointTC->StringWidth(_T("Default mount point:")) + 5); // TDEFAULTMOUNTPOINT"
	mountpointTC->GetButton()->SetLabel(_T("Browse"));
	mountpointTC->Init(_T("Select"), NULL, B_FILE_NODE | B_DIRECTORY_NODE);
	mountpointTC->GetTextControl()->SetLabel(_T("Default mount point:")); // TDEFAULTMOUNTPOINT"
	mountpointTC->GetTextControl()->TextView()->SetMaxBytes(8192);
	AddChild(mountpointTC);
}

PathConfigView::~PathConfigView()
{
}

const char* PathConfigView::GetTemporaryPath()
{
	return temppathTC->GetTextControl()->Text();
}

void PathConfigView::SetTemporaryPath(const char* path)
{
	temppathTC->GetTextControl()->SetText(path);
}

const char* PathConfigView::GetProjectPath()
{
	// printf("Project Path: %s\n", projectpathTC->GetTextControl()->Text());
	return projectpathTC->GetTextControl()->Text();
}

void PathConfigView::SetProjectPath(const char* path)
{
	projectpathTC->GetTextControl()->SetText(path);
}

const char* PathConfigView::GetDefaultImageName()
{
	return imagenameTC->GetTextControl()->Text();
}

void PathConfigView::SetDefaultImageName(const char* path)
{
	imagenameTC->GetTextControl()->SetText(path);
}

const char* PathConfigView::GetDefaultMountPoint()
{
	return mountpointTC->GetTextControl()->Text();
}

void PathConfigView::SetDefaultMountPoint(const char* path)
{
	mountpointTC->GetTextControl()->SetText(path);
}

// PRIVATE

void PathConfigView::AllAttached()
{
	// projectpathTC->GetTextControl()->SetTarget(this);
	// temppathTC->GetTextControl()->SetTarget(this);
	ConfigView::AllAttached();
}

void PathConfigView::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
	/*	case PROJECTPATH_CHANGED: {
			BString		bf=GetProjectPath();

			if (bf[bf.Length()-1]=='/')
				bf.RemoveLast("/");

			bf.Append("/");

			create_directory(bf.String(), 0777);
		}

		case TEMPPATH_CHANGED: {
			BString		bf=GetTemporaryPath();

			if (bf[bf.Length()-1]=='/')
				bf.RemoveLast("/");

			bf.Append("/");

			create_directory(bf.String(), 0777);
		}
*/
	default: {
		BView::MessageReceived(msg);
		break;
	}
	}
}

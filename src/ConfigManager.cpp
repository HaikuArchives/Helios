#include "ConfigManager.h"
#include <stdio.h>
#include <TypeConstants.h>
#include <Application.h>
#include <AppFileInfo.h>
#include <app/Roster.h>
#include <Bitmap.h>

ConfigManager::ConfigManager(const char* filename, bool reg)
{
	initcheck = B_OK;
	configMsg = new BMessage();
	configfile = new BFile(filename, B_READ_WRITE | B_CREATE_FILE);
	ReadConfiguration();
	registermime = reg;
}

ConfigManager::~ConfigManager()
{
	delete configMsg;
	delete configfile;
}

status_t ConfigManager::InitCheck()
{
	return initcheck;
}

bool ConfigManager::HasData(const char* name)
{
	return configMsg->GetInfo(name, &codeFound) == B_OK;
}

// BOOL
void ConfigManager::SetBool(const char* name, bool value)
{
	configMsg->RemoveName(name);
	configMsg->AddBool(name, value);
}

bool ConfigManager::GetBool(const char* name)
{
	bool value;
	if (configMsg->FindBool(name, &value) != B_OK) value = false;
	return value;
}

// INT64
void ConfigManager::SetInt64(const char* name, int64 value, int32 index)
{
	if (configMsg->ReplaceInt64(name, index, value) != B_OK) configMsg->AddInt64(name, value);
}

int64* ConfigManager::GetInt64(const char* name, int32 index)
{
	int64 value;
	if (configMsg->FindInt64(name, index, &value) != B_OK) return NULL;
	return new int64(value);
}

// INT32
void ConfigManager::SetInt32(const char* name, int32 value)
{
	configMsg->RemoveName(name);
	configMsg->AddInt32(name, value);
}

int32 ConfigManager::GetInt32(const char* name)
{
	int32 value;
	configMsg->FindInt32(name, &value);
	return value;
}

// INT16
void ConfigManager::SetInt16(const char* name, int16 value)
{
	configMsg->RemoveName(name);
	configMsg->AddInt16(name, value);
}

int16 ConfigManager::GetInt16(const char* name)
{
	int16 value;
	configMsg->FindInt16(name, &value);
	return value;
}

// INT8
void ConfigManager::SetInt8(const char* name, int8 value)
{
	configMsg->RemoveName(name);
	configMsg->AddInt8(name, value);
}

int8 ConfigManager::GetInt8(const char* name)
{
	int8 value;
	configMsg->FindInt8(name, &value);
	return value;
}

// FLOAT
void ConfigManager::SetFloat(const char* name, float value, int32 index)
{
	if (configMsg->ReplaceFloat(name, index, value) != B_OK) configMsg->AddFloat(name, value);
}

float* ConfigManager::GetFloat(const char* name, int32 index)
{
	float value;
	if (configMsg->FindFloat(name, index, &value) != B_OK) return NULL;
	return new float(value);
}

// RECT
void ConfigManager::SetRect(const char* name, BRect value)
{
	configMsg->RemoveName(name);
	configMsg->AddRect(name, value);
}

BRect* ConfigManager::GetRect(const char* name)
{
	static BRect* value = new BRect();
	configMsg->FindRect(name, value);
	return value;
}

// rgb_color
void ConfigManager::SetColor(const char* name, rgb_color value)
{
	configMsg->RemoveName(name);
	configMsg->AddData(name, B_RGB_COLOR_TYPE, &value, sizeof(value));
}

rgb_color ConfigManager::GetColor(const char* name)
{
	rgb_color* color;
	ssize_t length;

	configMsg->FindData(name, B_RGB_COLOR_TYPE, (const void**)(&color), &length);
	return *color;
}

// BMessage
void ConfigManager::SetMessage(const char* name, BMessage* msg)
{
	configMsg->RemoveName(name);
	configMsg->AddMessage(name, msg);
}

BMessage* ConfigManager::GetMessage(const char* name)
{
	static BMessage* msg = new BMessage();
	configMsg->FindMessage(name, msg);
	return msg;
}

// STRING
void ConfigManager::SetString(const char* name, BString* value)
{
	configMsg->RemoveName(name);
	configMsg->AddString(name, *value);
}

void ConfigManager::SetString(const char* name, const char* value)
{
	configMsg->RemoveName(name);
	configMsg->AddString(name, value);
}

BString* ConfigManager::GetString(const char* name)
{
	static BString* value = new BString("");
	configMsg->FindString(name, value);
	return value;
}

// STRING WITH INDICES
void ConfigManager::SetStringI(const char* name, BString* value, int32 index)
{
	if (index + 1 > CountStrings(name))
		configMsg->AddString(name, *value);
	else
		configMsg->ReplaceString(name, index, *value);
}

void ConfigManager::SetStringI(const char* name, const char* value, int32 index)
{
	if (index + 1 > CountStrings(name))
		configMsg->AddString(name, value);
	else
		configMsg->ReplaceString(name, index, value);
}

BString* ConfigManager::GetStringI(const char* name, int32 index)
{
	static BString* value = new BString("");
	configMsg->FindString(name, index, value);
	return value;
}

int32 ConfigManager::CountStrings(const char* name)
{
	int32 count = 0;
	type_code code;
	configMsg->GetInfo(name, &code, &count);
	return count;
}

void ConfigManager::RemoveString(const char* name, int32 index)
{
	configMsg->RemoveData(name, index);
}

// set file type database entry for Helios project files
void RegisterMIMEType()
{
	app_info appinfo;
	BBitmap* licon, *micon;

	be_app->GetAppInfo(&appinfo);
	BAppFileInfo* appfileinfo = new BAppFileInfo(new BFile(&appinfo.ref, B_READ_WRITE));
	BMimeType* projectmime = new BMimeType("application/x-vnd.Helios-project");

	licon = new BBitmap(BRect(0, 0, 31, 31), B_CMAP8);
	appfileinfo->GetIcon(licon, B_LARGE_ICON);
	projectmime->SetIconForType("application/x-vnd.Helios-project", licon, B_LARGE_ICON);
	projectmime->SetIcon(licon, B_LARGE_ICON);
	micon = new BBitmap(BRect(0, 0, 15, 15), B_CMAP8);
	appfileinfo->GetIcon(micon, B_MINI_ICON);
	projectmime->SetIconForType("application/x-vnd.Helios-project", micon, B_MINI_ICON);
	projectmime->SetIcon(micon, B_MINI_ICON);
	projectmime->SetPreferredApp(appinfo.signature);
	projectmime->SetLongDescription("Helios CDRECORDer Project File");
	projectmime->SetShortDescription("Helios Project");
	projectmime->Install();

	delete projectmime;
	delete appfileinfo;
	delete licon;
	delete micon;
}

void ConfigManager::ReadConfiguration()
{
	if ((initcheck = configfile->InitCheck()) == B_OK) {
		off_t filesize = 0;
		configfile->GetSize(&filesize);
		if (filesize > 0) {
			configfile->Seek(0, SEEK_SET);
			if (configMsg->Unflatten(configfile) == B_BAD_VALUE) {
				configfile->SetSize(0);
			}
		} else {
			if (registermime) RegisterMIMEType();
		}
	}
}

void ConfigManager::WriteConfiguration()
{
	if ((initcheck = configfile->InitCheck()) == B_OK) {
		configfile->Seek(0, SEEK_SET);
		configMsg->Flatten(configfile);
	}
}

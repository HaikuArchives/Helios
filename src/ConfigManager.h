#ifndef _CONFIGMANAGER_H_
#define _CONFIGMANAGER_H_

#include "ConfigWindow.h"
#include "ConfigView.h"
#include "HeliosConfigView.h"
#include <Message.h>
#include <File.h>
#include <String.h>

class ConfigManager {
public:
	ConfigManager(const char *filename, bool reg=false);
	~ConfigManager();
	
	status_t InitCheck();
	
	bool	HasData(const char *name);

	void	SetBool(const char *name, bool value);
	bool	GetBool(const char *name);

	void	SetInt64(const char *name, int64 value, int32 index=0);
	int64	*GetInt64(const char *name, int32 index=0);

	void	SetInt32(const char *name, int32 value);
	int32	GetInt32(const char *name);

	void	SetInt16(const char *name, int16 value);
	int16	GetInt16(const char *name);

	void	SetInt8(const char *name, int8 value);
	int8	GetInt8(const char *name);

	void	SetFloat(const char *name, float value, int32 index=0);
	float	*GetFloat(const char *name, int32 index=0);

	void	SetRect(const char *name, BRect value);
	BRect	*GetRect(const char *name);

	void	SetColor(const char *name, rgb_color value);
	rgb_color	GetColor(const char *name);

	void		SetMessage(const char *name, BMessage *msg);
	BMessage	*GetMessage(const char *name);

	void	SetString(const char *name, BString *value);
	void	SetString(const char *name, const char *value);
	BString	*GetString(const char *name);
	
	void	SetStringI(const char *name, BString *value, int32 index);
	void	SetStringI(const char *name, const char *value, int32 index);
	BString	*GetStringI(const char *name, int32 index);
	int32	CountStrings(const char *name);
	void	RemoveString(const char *name, int32 index);

	void	ReadConfiguration();
	void	WriteConfiguration();
	
private:
	BFile		*configfile;
	BMessage	*configMsg;
	status_t	initcheck;
	type_code	codeFound;
	bool		registermime;
};


#endif

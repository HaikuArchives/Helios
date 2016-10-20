#ifndef _FILESYSTEMPANEL_H_
#define _FILESYSTEMPANEL_H_

#include <InterfaceKit.h>
#include "ConfigView.h"
#include "BrowseField.h"

class FSPanel : public ConfigView {
public:
	FSPanel(BRect frame);
	~FSPanel();
	
	// Get...()
	bool	GetISO9660()			{return ISO9660_CB->Value()==B_CONTROL_ON;}
	bool	GetJoliet()			{return Joliet_CB->Value()==B_CONTROL_ON;}
	bool	GetRockRidge()		{return RockRidge_CB->Value()==B_CONTROL_ON;}
	bool	GetUDF()			{return UDF_CB->Value()==B_CONTROL_ON;}
	bool	GetHFS()			{return HFS_CB->Value()==B_CONTROL_ON;}
	bool	GetMapping()			{return mappingCB->Value()==B_CONTROL_ON;}
	const char *GetMappingFile()		{return strdup(mappingBF->GetTextControl()->Text());}
	
	bool	GetISO96608Dot3()		{return F83_RB->Value()==B_CONTROL_ON;}
	bool	GetISO966031()		{return F31_RB->Value()==B_CONTROL_ON;}
	bool	GetISO966037()		{return F37_RB->Value()==B_CONTROL_ON;}

	bool	GetJolietDefault()		{return JolietDef_RB->Value()==B_CONTROL_ON;}
	bool	GetJolietUnicode()		{return JolietUni_RB->Value()==B_CONTROL_ON;}

	bool	GetRockRidgeDefault()	{return RRDef_RB->Value()==B_CONTROL_ON;}
	bool	GetRockRidgeRationalized()	{return RRRat_RB->Value()==B_CONTROL_ON;}

	bool	GetUDFDefault()		{return UDFDef_RB->Value()==B_CONTROL_ON;}
	bool	GetUDFDVDVideo()		{return UDFDVD_RB->Value()==B_CONTROL_ON;}

	// Set...()
	void	SetISO9660(bool state)			{ISO9660_CB->SetValue(state?B_CONTROL_ON:B_CONTROL_OFF);}
	void	SetJoliet(bool state)				{Joliet_CB->SetValue(state?B_CONTROL_ON:B_CONTROL_OFF);}
	void	SetRockRidge(bool state)			{RockRidge_CB->SetValue(state?B_CONTROL_ON:B_CONTROL_OFF);}
	void	SetUDF(bool state)				{UDF_CB->SetValue(state?B_CONTROL_ON:B_CONTROL_OFF);}
	void	SetHFS(bool state)				{HFS_CB->SetValue(state?B_CONTROL_ON:B_CONTROL_OFF);}
	void	SetMapping(bool state)			{mappingCB->SetValue(state?B_CONTROL_ON:B_CONTROL_OFF);}
	void	SetMappingFile(const char *file)		{mappingBF->GetTextControl()->SetText(file);}
	
	void	SetISO96608Dot3(bool state)		{F83_RB->SetValue(state?B_CONTROL_ON:B_CONTROL_OFF);}
	void	SetISO966031(bool state)			{F31_RB->SetValue(state?B_CONTROL_ON:B_CONTROL_OFF);}
	void	SetISO966037(bool state)			{F37_RB->SetValue(state?B_CONTROL_ON:B_CONTROL_OFF);}

	void	SetJolietDefault(bool state)			{JolietDef_RB->SetValue(state?B_CONTROL_ON:B_CONTROL_OFF);}
	void	SetJolietUnicode(bool state)			{JolietUni_RB->SetValue(state?B_CONTROL_ON:B_CONTROL_OFF);}

	void	SetRockRidgeDefault(bool state)		{RRDef_RB->SetValue(state?B_CONTROL_ON:B_CONTROL_OFF);}
	void	SetRockRidgeRationalized(bool state)	{RRRat_RB->SetValue(state?B_CONTROL_ON:B_CONTROL_OFF);}

	void	SetUDFDefault(bool state)			{UDFDef_RB->SetValue(state?B_CONTROL_ON:B_CONTROL_OFF);}
	void	SetUDFDVDVideo(bool state)		{UDFDVD_RB->SetValue(state?B_CONTROL_ON:B_CONTROL_OFF);}
	
	
	BList	*GetArgumentList();
	
	
	
protected:

	virtual void	MessageReceived(BMessage *);
	virtual void	AllAttached();

private:
	BCheckBox	*ISO9660_CB,
			*Joliet_CB,
			*RockRidge_CB,
			*UDF_CB,
			*HFS_CB,
			*Apple_CB,
			*BFS_CB,
			*mappingCB;
	BRadioButton	*F83_RB,
			*F31_RB,
			*F37_RB,
			*JolietDef_RB,
			*JolietUni_RB,
			*RRDef_RB,
			*RRRat_RB,
			*UDFDef_RB,
			*UDFDVD_RB;
	BView		*ISO9660_V,
			*Joliet_V,
			*RockRidge_V,
			*UDF_V,
			*HFS_V,
			*BFS_V;
	BrowseField	*mappingBF;
	
	void		CheckCB();
};


#endif


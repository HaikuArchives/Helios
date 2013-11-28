#ifndef _DEVICECONFIGVIEW_H_
#define _DEVICECONFIGVIEW_H_

#include "ConfigView.h"
#include <Slider.h>
#include <CheckBox.h>
#include <TextControl.h>
#include <String.h>
#include <StringView.h>
#include <MenuField.h>
#include <PopUpMenu.h>
#include <InterfaceKit.h>


#define DCV_DEFAULT_METHOD		0
#define DCV_SECOND_METHOD		1
#define DCV_EMERGENCY_METHOD	2

class DeviceConfigView : public ConfigView {
public:
	DeviceConfigView(BRect frame);
	~DeviceConfigView();
	
	void	RescanDevices();
	
	bool	IsTwoDriveCopy();
	void	SetTwoDriveCopy(bool);

	bool	IsWriterEjected();
	void	SetWriterEjected(bool);
	
	bool	IsReaderEjected();
	void	SetReaderEjected(bool);

	
	const char *GetWriterDeviceNumber();
	const char *GetReaderDeviceNumber();	
	const char *GetWriterDevicePath();
	const char *GetReaderDevicePath();
	
	int	GetWriterNumber();
	void	SetWriterNumber(int);
	
	int	GetReaderNumber();
	void	SetReaderNumber(int);
	
	int8	GetScanMethod();
	void	SetScanMethod(int8);
	
	bool	GetShowsAllDevices() const;
	void	SetShowAllDevices(bool);

private:
	
	void	AllAttached();
	void	MessageReceived(BMessage*);
	void	lookup_devices(const char *path, const char *appendthis);
	
	BCheckBox	*twodrivecopyCB,
			*writerejectedCB,
			*readerejectedCB;
	
	BBox		*scanmethodBOX;
	BRadioButton	*defaultRB,
			*secondRB,
			*emergencyRB;
	BCheckBox	*showallCB;
	
	BMenuField	*writerMF,
			*readerMF;
	BPopUpMenu	*writerPUM,
			*readerPUM;
	const char 	**devices;
	const char 	**devicepaths;
	int		number;
	int8		fMethod;
};

#endif

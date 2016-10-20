#include <strings.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <Path.h>
#include <Directory.h>
#include <Entry.h>
#include <MenuItem.h>

#if defined(_BEOS_R5_BUILD_) || defined(_BEOS_HAIKU_BUILD_)
#include "MSHLanguageMgr.h"
extern MSHLanguageMgr* gMSHLangMgr;
#define _T(str) gMSHLangMgr->_T(str).String()
#define _TPS(str) gMSHLangMgr->_T(str)
#else
#include <locale/Locale.h>
#endif

#include <CAM.h>
#include <scsi.h>

#include "DeviceConfigView.h"

#define DEVICECHANGED 'DCV0'
#define METHODCHANGED 'DCV1'

DeviceConfigView::DeviceConfigView(BRect frame) : ConfigView(frame)
{
	// calcualte string lengths and set dividers accordingly
	float width = 0.0;
	width = StringWidth(_T( "CD Writer" ));
	if (width < StringWidth(_T( "CD Reader" ))) width = StringWidth(_T( "CD Reader" ));
	width += 5;

	// CD Writer MenuField
	writerPUM = new BPopUpMenu("                            ");
	writerMF =
		new BMenuField(BRect(15, 5, 290, 25), "dcv_cdwriterMF", _T("CD Writer"), // "TCDRDRIVE"
					   writerPUM);
	writerMF->SetDivider(width);
	AddChild(writerMF);

	// CD Reader MenuField
	readerPUM = new BPopUpMenu("                            ");
	readerMF = new BMenuField(BRect(15, writerMF->Frame().bottom + 5, 290, 50), "dcv_cdreaderMF",
							  _T("CD Reader"), // "TCDREADERLABEL"
							  readerPUM);
	readerMF->SetDivider(width);
	AddChild(readerMF);

	// CHECKBOXES
	writerejectedCB = new BCheckBox(BRect(310, 7, Bounds().right - 15, 27), "dcv_ejectwriterCB",
									_T("Eject"), // "TEJECT"
									NULL);
	writerejectedCB->ResizeToPreferred();
	AddChild(writerejectedCB);
	readerejectedCB = new BCheckBox(BRect(310, 32, Bounds().right - 15, 52), "dcv_ejectreaderCB",
									_T("Eject"), // "TEJECT"
									NULL);
	readerejectedCB->ResizeToPreferred();
	AddChild(readerejectedCB);

	twodrivecopyCB = new BCheckBox(BRect(15, 60, 230, 75), "dcv_twodrivecopyCB",
								   _T("2-Drive-Copy"), // "TTWODRIVECOPY"
								   NULL);
	AddChild(twodrivecopyCB);

	scanmethodBOX = new BBox(BRect(15, 90, Bounds().right - 15, 207));
	scanmethodBOX->SetLabel(_T("Device scan")); // "BOX:Device Scan"
	AddChild(scanmethodBOX);

	defaultRB = new BRadioButton(BRect(20, 20, scanmethodBOX->Bounds().right - 15, 35),
								 "dcv_defaultRB", _T("Default method"), // "RB:Default Method"
								 new BMessage(METHODCHANGED));
	scanmethodBOX->AddChild(defaultRB);
	secondRB = new BRadioButton(BRect(20, 40, scanmethodBOX->Bounds().right - 15, 55),
								"dcv_secondRB", _T("Second method"), // "RB:Second Method"
								new BMessage(METHODCHANGED));
	scanmethodBOX->AddChild(secondRB);
	emergencyRB =
		new BRadioButton(BRect(20, 60, scanmethodBOX->Bounds().right - 15, 75), "dcv_emergencyRB",
						 _T("Emergency method"), // "RB:Emergency Method"
						 new BMessage(METHODCHANGED));

	// NOT IMPLEMENTED
	emergencyRB->SetEnabled(false);
	// NOT IMPLEMENTED

	scanmethodBOX->AddChild(emergencyRB);

	showallCB = new BCheckBox(BRect(20, 85, 230, 100), "dcv_showallCB",
							  _T("Show all devices"), // "CB:Show All Devices"
							  new BMessage(METHODCHANGED));
	scanmethodBOX->AddChild(showallCB);

	devices = (const char**)malloc(sizeof(char*) * 65);
	devicepaths = (const char**)malloc(sizeof(char*) * 65);
	RescanDevices();
}

DeviceConfigView::~DeviceConfigView()
{
	free(devices);
	free(devicepaths);
}

void DeviceConfigView::RescanDevices()
{
	number = 0;

	for (int32 i = readerPUM->CountItems() - 1; i >= 0; i--) delete readerPUM->RemoveItem(i);
	for (int32 i = writerPUM->CountItems() - 1; i >= 0; i--) delete writerPUM->RemoveItem(i);

	switch (GetScanMethod()) {
	case DCV_DEFAULT_METHOD: {
		lookup_devices("/dev/disk/scsi", " (SCSI)");
#ifdef _BEOS_HAIKU_BUILD_
		lookup_devices("/dev/disk/atapi", " (ATAPI)");
#else
		lookup_devices("/dev/disk/ide", " (IDE)");
#endif
		break;
	}

	case DCV_SECOND_METHOD: {
		lookup_devices("/dev/bus/scsi", " (SCSI)");
		lookup_devices("/dev/disk/ide", " (IDE)");
		break;
	}

	case DCV_EMERGENCY_METHOD: {
		break;
	}

	default: {
		break;
	}
	}
	if ((writerPUM->CountItems() > 0) && (writerPUM->FindMarked() == NULL)) {
		writerPUM->ItemAt(0)->SetMarked(true);
	}
	if ((readerPUM->CountItems() > 0) && (readerPUM->FindMarked() == NULL)) {
		readerPUM->ItemAt(0)->SetMarked(true);
	}
}

bool DeviceConfigView::IsTwoDriveCopy()
{
	if (GetReaderNumber() != GetWriterNumber())
		return twodrivecopyCB->Value() == B_CONTROL_ON;
	else
		return false;
}

void DeviceConfigView::SetTwoDriveCopy(bool state)
{
	twodrivecopyCB->SetValue(state ? B_CONTROL_ON : B_CONTROL_OFF);
}

bool DeviceConfigView::IsWriterEjected()
{
	return writerejectedCB->Value() == B_CONTROL_ON;
}

void DeviceConfigView::SetWriterEjected(bool state)
{
	writerejectedCB->SetValue(state ? B_CONTROL_ON : B_CONTROL_OFF);
}

bool DeviceConfigView::IsReaderEjected()
{
	return readerejectedCB->Value() == B_CONTROL_ON;
}

void DeviceConfigView::SetReaderEjected(bool state)
{
	readerejectedCB->SetValue(state ? B_CONTROL_ON : B_CONTROL_OFF);
}

const char* DeviceConfigView::GetWriterDeviceNumber()
{
	return strdup(devices[writerPUM->IndexOf(writerPUM->FindMarked())]);
}

const char* DeviceConfigView::GetReaderDeviceNumber()
{
	return strdup(devices[readerPUM->IndexOf(readerPUM->FindMarked())]);
}

const char* DeviceConfigView::GetWriterDevicePath()
{
	return strdup(devicepaths[writerPUM->IndexOf(writerPUM->FindMarked())]);
}

const char* DeviceConfigView::GetReaderDevicePath()
{
	return strdup(devicepaths[readerPUM->IndexOf(readerPUM->FindMarked())]);
}

int DeviceConfigView::GetWriterNumber()
{
	return writerPUM->IndexOf(writerPUM->FindMarked());
}

void DeviceConfigView::SetWriterNumber(int no)
{
	if (writerPUM->ItemAt(no) != NULL) (writerPUM->ItemAt(no))->SetMarked(true);
}

int DeviceConfigView::GetReaderNumber()
{
	return readerPUM->IndexOf(readerPUM->FindMarked());
}

void DeviceConfigView::SetReaderNumber(int no)
{
	if (readerPUM->ItemAt(no) != NULL) (readerPUM->ItemAt(no))->SetMarked(true);
}

int8 DeviceConfigView::GetScanMethod()
{
	fMethod = 1;
	if (defaultRB->Value() == B_CONTROL_ON) fMethod = DCV_DEFAULT_METHOD;
	if (secondRB->Value() == B_CONTROL_ON) fMethod = DCV_SECOND_METHOD;
	if (emergencyRB->Value() == B_CONTROL_ON) fMethod = DCV_EMERGENCY_METHOD;
	return fMethod;
}

void DeviceConfigView::SetScanMethod(int8 method)
{
	fMethod = method;
	switch (fMethod) {
	case DCV_DEFAULT_METHOD: {
		defaultRB->SetValue(B_CONTROL_ON);
		break;
	}
	case DCV_SECOND_METHOD: {
		secondRB->SetValue(B_CONTROL_ON);
		break;
	}
	case DCV_EMERGENCY_METHOD: {
		emergencyRB->SetValue(B_CONTROL_ON);
		break;
	}

	default:
		break;
	}
}

bool DeviceConfigView::GetShowsAllDevices() const
{
	return showallCB->Value() == B_CONTROL_ON;
}

void DeviceConfigView::SetShowAllDevices(bool state)
{
	showallCB->SetValue(state ? B_CONTROL_ON : B_CONTROL_OFF);
}

// PRIVATE

void DeviceConfigView::AllAttached()
{
	writerPUM->SetTargetForItems(this);
	readerPUM->SetTargetForItems(this);
	twodrivecopyCB->SetEnabled(GetReaderNumber() != GetWriterNumber());
	defaultRB->SetTarget(this);
	secondRB->SetTarget(this);
	emergencyRB->SetTarget(this);
	showallCB->SetTarget(this);

	BView::AllAttached();
}

void DeviceConfigView::MessageReceived(BMessage* msg)
{
	switch (msg->what) {
	case DEVICECHANGED: {
		twodrivecopyCB->SetEnabled(GetReaderNumber() != GetWriterNumber());
		break;
	}

	case METHODCHANGED: {
		RescanDevices();
		break;
	}

	default: {
		BView::MessageReceived(msg);
		break;
	}
	}
}

// this is Philippe Houdoin's
status_t get_device_path_target_lun(const char* dev, int* path, int* target, int* lun)
{
	int p, t, l;

	p = t = l = 0;
	if (!strncmp("/dev/bus/scsi/", dev, 14))
		sscanf(dev, "/dev/bus/scsi/%d/%d/%d/raw", &p, &t, &l);
	else if (!strncmp("/dev/disk/scsi/", dev, 15))
		sscanf(dev, "/dev/disk/scsi/%d/%d/%d/raw", &p, &t, &l);
	else if (!strncmp("/dev/disk/ide/ata/", dev, 18)) {
		char ide_targ[64];

		sscanf(dev, "/dev/disk/ide/ata/%d/%s/%d/raw", &p, ide_targ, &l);
		p += 8;
		if (strncmp(ide_targ, "master", 6) == 0) t = 0;
		if (strncmp(ide_targ, "slave", 5) == 0) t = 1;
	} else if (!strncmp("/dev/disk/ide/atapi/", dev, 18)) {
		char ide_targ[64];

		sscanf(dev, "/dev/disk/ide/atapi/%d/%s/%d/raw", &p, ide_targ, &l);
		p += 8;
		if (strncmp(ide_targ, "master", 6) == 0) t = 0;
		if (strncmp(ide_targ, "slave", 5) == 0) t = 1;
	};

	*path = p;
	*target = t;
	*lun = l;
	return B_OK;
}

// this is Philippe Houdoin's
void DeviceConfigView::lookup_devices(const char* path, const char* appendthis)
{
	BDirectory dir(path);

	if (dir.InitCheck() != B_OK) return;

	BEntry entry;

	dir.Rewind();
	while (dir.GetNextEntry(&entry) >= 0) {
		BPath name;
		entry.GetPath(&name);
		if (entry.IsDirectory()) lookup_devices(name.Path(), appendthis);

		if (strcmp(name.Leaf(), "raw") != 0) continue;

		int fd, e;
		int path, targ, lun;
		char ide_targ[64];
		const char* dev;
		uchar data[36], sense[16];
		int type;
		raw_device_command rdc;

		dev = name.Path();
		path = targ = lun = 0;
		if (!strncmp("/dev/disk/scsi/", dev, 15))
			sscanf(dev, "/dev/disk/scsi/%d/%d/%d/raw", &path, &targ, &lun);
		else if (!strncmp("/dev/disk/ide/ata/", dev, 18)) {
			sscanf(dev, "/dev/disk/ide/ata/%d/%s/%d/raw", &path, ide_targ, &lun);
			if (strncmp(ide_targ, "master", 6) == 0) targ = 0;
			if (strncmp(ide_targ, "slave", 5) == 0) targ = 1;
		} else if (!strncmp("/dev/disk/ide/atapi/", dev, 18)) {
			sscanf(dev, "/dev/disk/ide/atapi/%d/%s/%d/raw", &path, ide_targ, &lun);
			if (strncmp(ide_targ, "master", 6) == 0) targ = 0;
			if (strncmp(ide_targ, "slave", 5) == 0) targ = 1;
		};

		// fill out our raw device command data
		rdc.data = data;
		rdc.data_length = 36;
		rdc.sense_data = sense;
		rdc.sense_data_length = 0;
		rdc.timeout = 1000000;
		rdc.flags = B_RAW_DEVICE_DATA_IN;
		rdc.command_length = 6;
		rdc.command[0] = 0x12;
		rdc.command[1] = 0x00;
		rdc.command[2] = 0x00;
		rdc.command[3] = 0x00;
		rdc.command[4] = 36;
		rdc.command[5] = 0x00;

		if ((fd = open(dev, 0)) < 0) continue;

		e = ioctl(fd, B_RAW_DEVICE_COMMAND, &rdc, sizeof(rdc));
		close(fd);
		if ((e != 0) || (rdc.cam_status != CAM_REQ_CMP)) {
			// printf("B_RAW_DEVICE_COMMAND(inquiry) failed on %s!\n", dev);
			continue;
		};

		type = data[0] & 0x1F;
		if (!GetShowsAllDevices())
			if (type != 5) // if not CD-ROM type
				continue;

		char vendor[32];
		char product[32];
		char devnumber[7];

		BString label;

		get_device_path_target_lun(dev, &path, &targ, &lun);
		sprintf(devnumber, "%d,%d,%d", path, targ, lun);
		devices[number] = strdup(devnumber);
		devicepaths[number++] = strdup(dev);
		sprintf(vendor, "%8.8s", data + 8);
		sprintf(product, "%16.16s", data + 16);

		label << vendor;
		label.RemoveAll(" ");

		label << " " << product << appendthis;

		writerPUM->AddItem(new BMenuItem(label.String(), new BMessage(DEVICECHANGED)));
		readerPUM->AddItem(new BMenuItem(label.String(), new BMessage(DEVICECHANGED)));

	}; // while
	dir.Unset();
	entry.Unset();
}

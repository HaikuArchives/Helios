#include "CDRECORDCmdLine.h"
#include "Application1.h"
#include "ConfigFields.h"
#include "ErrorBox.h"

status_t CDRECORD_CommandLine(const char** argv, int* argc, bool multisession, bool isStreaming)
{
	Application1* app = (Application1*)be_app;
	Window1* window1 = (Window1*)app->window1;
	View1* view1 = (View1*)window1->view1;
	BString str = "";

	argv[(*argc)++] = strdup(app->CM->GetString(PATH_TO_CDRECORD)->String());
	// device number
	str = "dev=";
	str << app->deviceCV->GetWriterDeviceNumber();
	argv[(*argc)++] = strdup(str.String());

	// verbose mode
	argv[(*argc)++] = strdup("-v");

	BString driveroptsStr("");

	// user arguments
	BString userFlags = app->heliosCV->GetGeneralFlags().String();
	BList argList;
	CDRECORD_ParseUserFlags(userFlags, argList);

	while (argList.CountItems() > 0) {
		BString* item = (BString*)argList.ItemAt(0);

		if (item->FindFirst("driveropts=") != B_ERROR) {
			// Save driveropts for later addition.
			if (driveroptsStr.Length() > 0) {
				driveroptsStr += ",";
			}
			driveroptsStr += item->RemoveFirst("driveropts=");
		} else {
			argv[(*argc)++] = strdup(item->String());
		}
		argList.RemoveItem(static_cast<int32>(0));
		delete item;
		item = NULL;
	}

	// burnproof
	if (app->heliosCV->IsBurnproof()) {
		if (driveroptsStr.Length() > 0) {
			driveroptsStr += ",";
		}
		driveroptsStr += "burnproof";
	}

	// driveropts all at once.
	driveroptsStr.Prepend("driveropts=");
	argv[(*argc)++] = strdup(driveroptsStr.String());

	// overburning
	if (app->heliosCV->IsOverburning()) argv[(*argc)++] = strdup("-overburn");

	// FIFO buffer size
	if (app->heliosCV->GetFIFOSize() != 4) {
		str.SetTo("fs=");
		str << app->heliosCV->GetFIFOSize() << "M";
		argv[(*argc)++] = strdup(str.String());
	}

	if (isStreaming) {
		// wait for data input stream
		argv[(*argc)++] = strdup("-waiti");
	}

	str = "-speed=";
	str << app->heliosCV->GetWriterSpeed();
	argv[(*argc)++] = strdup(str.String());

	if (!app->heliosCV->IsFixating()) {
		argv[(*argc)++] = strdup("-nofix");
	}

	if (app->heliosCV->IsSimulation()) {
		argv[(*argc)++] = strdup("-dummy");
	}

	switch (view1->CDType()) {
	case CDEXTRA_INDEX: {
		break;
	}

	default: {
		if (multisession) {
			argv[(*argc)++] = strdup("-multi");
		}
		if (app->heliosCV->IsDAO()) {
			argv[(*argc)++] = strdup("-dao");
		}
		break;
	}
	}

	return B_OK;
}

void CDRECORD_ParseUserFlags(BString userFlags, BList& argList)
{
	const char spaceChar = 32;
	const char tabChar = 9;

	// Leading whitespace removal.
	while (userFlags.Length() > 0) {
		if ((userFlags[0] == spaceChar) || (userFlags[0] == tabChar)) {
			userFlags.Remove(0, 1);
			continue;
		}

		// Obtain the next argument and allocate it in the list.
		BString* nextArg = new BString("");
		const int32 nextSpace = userFlags.FindFirst(spaceChar);
		const int32 nextTab = userFlags.FindFirst(tabChar);

		if (nextSpace != B_ERROR) {
			userFlags.MoveInto(*nextArg, 0, nextSpace);
		} else if (nextTab != B_ERROR) {
			userFlags.MoveInto(*nextArg, 0, nextTab);
		} else {
			// No more arguments after this one - so take until end of string.
			*nextArg = userFlags;
			userFlags = "";
		}

		argList.AddItem(nextArg);
	}
}

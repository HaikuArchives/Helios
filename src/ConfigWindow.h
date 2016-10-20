#ifndef _CONFIGWINDOW_H_
#define _CONFIGWINDOW_H_

#include <Window.h>
#include <OutlineListView.h>
#include <View.h>
#include <Box.h>
#include "ConfigView.h"
#include <List.h>
#include <ScrollView.h>
#include <StringView.h>
#include "ConfigHeadLine.h"

class ConfigWindow : public BWindow
{
public:
	ConfigWindow(const char* title);
	~ConfigWindow();

	void AddConfigView(const char* name, BView* view);
	void AddTopic(const char* name);

	int32 GetConfigIndex(const char* name);
	const char* GetConfigName(int32 index);

	BView* GetConfigView(int32 index);
	BView* GetConfigView(const char* name) { return this->GetConfigView(GetConfigIndex(name)); };

	BRect GetConfigViewFrame();

	void Show();
	bool SelectView(int32 index);

private:
	void ActivateView(int32 index);
	void RefreshConfigView();
	void MessageReceived(BMessage* msg);
	bool QuitRequested();

	BOutlineListView* selectionLV;
	BView* mainV;
	BView* boundingBOX;
	ConfigView* emptyCV;
	BList* viewsL;
	BScrollView* scrollSV;
	BListItem* superLI;
	ConfigHeadLine* labelCHL;
};

#endif

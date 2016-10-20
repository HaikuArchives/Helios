#ifndef _DATACDPANEL_H_
#define _DATACDPANEL_H_
#include <View.h>
#include <CheckBox.h>
#include <PopUpMenu.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <TextControl.h>

class DataCDPanel : public BView {
public:
	DataCDPanel(BRect frame);
	~DataCDPanel();
	
	void		AllAttached(void);
//	const char	*GetFileSystem();
	bool		IsMultisession();
	void		SetMultisession(bool flag);
	const char	*GetVolumeName(void);
	void		SetVolumeName(const char *name);
	int		GetFilesystemIndex();
	BList		*GetFilesystem();

	BPopUpMenu	*filesystemPUM;

protected:
	virtual void	MessageReceived(BMessage *);
	
private:
	BCheckBox	*multisessionCB;
	BMenuField	*filesystemMF;
	BMenuItem	*mi;
	BTextControl	*volumenameTC;
};

#endif

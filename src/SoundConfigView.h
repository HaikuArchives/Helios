#ifndef _SOUNDCONFIGVIEW_H_
#define _SOUNDCONFIGVIEW_H_

#include "ConfigView.h"
#include <Slider.h>
#include <CheckBox.h>
#include <TextControl.h>
#include <String.h>
#include <StringView.h>
#include <RadioButton.h>


class SoundConfigView : public ConfigView {
public:
	SoundConfigView(BRect frame);
	~SoundConfigView();
	
	bool	HasFinishedWritingSound();
	void	SetFinishedWritingSound(bool);
	
	bool	HasFinishedImageSound();
	void	SetFinishedImageSound(bool);
	
	bool	HasErrorSound();
	void	SetErrorSound(bool);

	// following to be implemented later...
	const char	*GetFinishedWritingSoundFile();
	void		SetFinishedWritingSoundFile(const char *);
	
	const char	*GetFinishedImageSoundFile();
	void		SetFinishedImageSoundFile(const char *);

private:
	
	void	MessageReceived(BMessage*);
	
	BCheckBox	*writtensoundCB,
			*imagesoundCB,
			*errorsoundCB;
	BString		string;
};

#endif

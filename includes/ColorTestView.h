#ifndef _COLORTESTVIEW_H_
#define _COLORTESTVIEW_H_

#include <InterfaceKit.h>

#define CTV_COLOR_UPDATE_MSG	'CTV0'

class ColorTestView : public BView {
	public:
		ColorTestView(BRect, const char *);
		~ColorTestView();

		void	SetTestColor(rgb_color);

	protected:
		virtual void	MessageReceived(BMessage *msg);
		virtual void	Draw(BRect);
		void			DrawColorArea();
	
	private:
		rgb_color	fColor;
};

#endif

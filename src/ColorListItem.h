#ifndef _COLORLISTITEM_H_
#define _COLORLISTITEM_H_


#include <InterfaceKit.h>

class ColorListItem : public BStringItem {
	public:
		ColorListItem(const char *text, uint32 level = 0, bool expanded = true);
		~ColorListItem();
		
		virtual void	DrawItem(BView *owner, BRect itemRect, bool drawEverything=false);
		
		void		SetColor(rgb_color color);
		rgb_color	GetColor() const;
		
		void		SetFrameColor(rgb_color);
		void		SetFrameSize(BRect frame);
		void		SetFrameAlignment(alignment);
	
	protected:
	
	private:
		rgb_color	fColor;
		rgb_color	fFrameColor;
		BRect		fColorFrame;
		alignment	fColorFrameAlign;
};

#endif

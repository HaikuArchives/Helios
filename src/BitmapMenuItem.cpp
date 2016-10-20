//--------------------------------------------------------------------
//
//	BitmapMenuItem.cpp
//
//	Written by: Owen Smith
//	modified by maurice
//--------------------------------------------------------------------

/*
	Copyright 1999, Be Incorporated.   All Rights Reserved.
	This file may be used under the terms of the Be Sample Code License.
*/
#include <GraphicsDefs.h>
#include "BitmapMenuItem.h"
const rgb_color BKG_GREY = {216, 216, 216, 255};

//====================================================================
//	BitmapMenuItem Implementation

//--------------------------------------------------------------------
//	BitmapMenuItem constructors, destructors, operators

BitmapMenuItem::BitmapMenuItem(const char* name, const BBitmap& bitmap, BMessage* message,
							   char shortcut, uint32 modifiers)
	: BMenuItem(name, message, shortcut, modifiers), m_bitmap(bitmap.Bounds(), bitmap.ColorSpace())
{
	// Sadly, operator= for bitmaps is not yet implemented.
	// Half of m_bitmap's initialization is above; now we copy
	// the bits.
	m_bitmap.SetBits(bitmap.Bits(), bitmap.BitsLength(), 0, bitmap.ColorSpace());

	m_Text = name;
}

//--------------------------------------------------------------------
//	BitmapMenuItem constructors, destructors, operators

void BitmapMenuItem::Draw(void)
{
	BMenu* menu = Menu();
	if (menu) {
		BRect itemFrame = Frame();
		BRect bitmapFrame = m_bitmap.Bounds();
		bitmapFrame.OffsetBy(12, 2 + itemFrame.top); // account for 2-pixel margin

		menu->SetDrawingMode(B_OP_OVER);
		menu->SetHighColor(BKG_GREY);
		menu->FillRect(itemFrame);
		menu->DrawBitmap(&m_bitmap, bitmapFrame);
		menu->SetHighColor(0, 0, 0, 255);
		menu->DrawString(m_Text.String(), BPoint(5 + bitmapFrame.right, bitmapFrame.bottom - 4));

		if (IsSelected()) {
			// a nonstandard but simple way to draw highlights
			menu->SetDrawingMode(B_OP_OVER);
			menu->SetHighColor(154, 154, 154, 11);
			menu->FillRect(itemFrame);
			menu->DrawBitmap(&m_bitmap, bitmapFrame);
			menu->SetHighColor(0, 0, 0, 255);
			menu->DrawString(m_Text.String(),
							 BPoint(5 + bitmapFrame.right, bitmapFrame.bottom - 4));
		}
	}
}

void BitmapMenuItem::GetContentSize(float* width, float* height)
{
	GetBitmapSize(width, height);
}

//--------------------------------------------------------------------
//	BitmapMenuItem accessors

void BitmapMenuItem::GetBitmapSize(float* width, float* height)
{
	BRect r = m_bitmap.Bounds();
	*width = r.Width() + 14 + Menu()->StringWidth(m_Text.String());
	// +10 :  space left of the bitmap
	*height = r.Height();
}

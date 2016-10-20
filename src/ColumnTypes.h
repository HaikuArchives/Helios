/*******************************************************************************
/
/	File:			ColumnTypes.h
/
/   Description:    Experimental classes that implement particular column/field
/					data types for use in BColumnListView.
/
/	Copyright 2000+, Be Incorporated, All Rights Reserved
/
*******************************************************************************/


#ifndef _COLUMN_TYPES_H
#define _COLUMN_TYPES_H

#include "ColumnListView.h"
#include <String.h>
#include <Font.h>
#include <Bitmap.h>
#include <TextView.h>		//HUGH for BStringColumn's edit-view

//=====================================================================
// Common base-class: a column that draws a standard title at its top.

class BTitledColumn : public BColumn
{
	public:
							BTitledColumn		(const char *title,
												 float width,
												 float minWidth,
												 float maxWidth,
												 alignment align = B_ALIGN_LEFT);
		virtual void		DrawTitle			(BRect rect,
												 BView* parent);
		virtual void		GetColumnName		(BString* into) const;

		void				DrawString			(const char*,
												 BView*,
												 BRect);
		void				SetTitle			(const char* title);
		void				Title				(BString* forTitle) const; // sets the BString arg to be the title
		float				FontHeight			() const;

	private:
		float				fFontHeight;
		BString				fTitle;
};


//=====================================================================
// Field and column classes for strings.

class BStringField : public BField
{
	public:
									BStringField		(const char* string);

				void				SetString			(const char* string);
				const char*			String				() const;
				void				SetClippedString	(const char* string);
				const char*			ClippedString		();
				void				SetWidth			(float);
				float				Width				();
	
	private:
		float				fWidth;
		BString				fString;
		BString				fClippedString;
};


//--------------------------------------------------------------------
//HUGH TextView for editing a BStringColumn
class BStringEditView : public BTextView
{
	public:
	//! \param master The ColumnListView the editview is attached to, for navigation
	//! \param flags  the edit-flags, see enum EditFlags in ColumnListView.h
	BStringEditView(BColumnListView* master, int flags);
	~BStringEditView();
	
	// filter Enter, CrsUp/Down, PgUp/Dwn, TAB
	virtual void        KeyDown         ( const char* bytes, int32 numBytes);
	int                 GetEditFlags()  { return fEditFlags; }
	
	private:
	BColumnListView*    fMasterView;      // for navigation
	int                 fEditFlags;
};
//-HUGH

class BStringColumn : public BTitledColumn
{
	public:
							BStringColumn		(const char *title,
												 float width,
												 float maxWidth,
												 float minWidth,
												 uint32 truncate,
												 alignment align = B_ALIGN_LEFT);
		virtual void		DrawField			(BField* field,
												 BRect rect,
												 BView* parent);
		virtual int			CompareFields		(BField* field1,
												 BField* field2);
		virtual	bool		AcceptsField        (const BField* field) const;
		//HUGH augmented BColumn::-versions to create a suitable textview for editing
		virtual void		AddEditView(BView* view, BField *field, BRect rect);
		virtual void		RemoveEditView(BField *field);

	private:
		uint32				fTruncate;
};


//=====================================================================
// Field and column classes for dates.

class BDateField : public BField
{
	public:
							BDateField			(time_t* t);
		void				SetWidth			(float);
		float				Width				();
		void				SetClippedString	(const char*);
		const char*			ClippedString		();
		time_t				Seconds				();
		time_t				UnixTime			();

	private:	
		struct tm			fTime;
		time_t				fUnixTime;
		time_t				fSeconds;
		BString				fClippedString;
		float				fWidth;
};


//--------------------------------------------------------------------

class BDateColumn : public BTitledColumn
{
	public:
							BDateColumn			(const char* title,
												 float width,
												 float minWidth,
												 float maxWidth,
												 alignment align = B_ALIGN_LEFT);
		virtual void		DrawField			(BField* field,
												 BRect rect,
												 BView* parent);
		virtual int			CompareFields		(BField* field1,
												 BField* field2);
	private:
		BString				fTitle;
};


//=====================================================================
// Field and column classes for numeric sizes.

class BSizeField : public BField
{
	public:
							BSizeField			(off_t size);
		void				SetSize				(off_t);
		off_t				Size				();

	private:
		off_t				fSize;
};


//--------------------------------------------------------------------

class BSizeColumn : public BTitledColumn
{
	public:
							BSizeColumn			(const char* title,
												 float width,
												 float minWidth,
												 float maxWidth,
												 alignment align = B_ALIGN_LEFT);
		virtual void		DrawField			(BField* field,
												 BRect rect,
												 BView* parent);
		virtual int			CompareFields		(BField* field1,
												 BField* field2);
};


//=====================================================================
// Field and column classes for integers.

class BIntegerField : public BField
{
	public:
							BIntegerField		(int32 number);
		void				SetValue			(int32);
		int32				Value				();

	private:
		int32				fInteger;
};


//--------------------------------------------------------------------

class BIntegerColumn : public BTitledColumn
{
	public:
							BIntegerColumn		(const char* title,
												 float width,
												 float minWidth,
												 float maxWidth,
												 alignment align = B_ALIGN_LEFT);
		virtual void		DrawField			(BField* field,
												 BRect rect,
												 BView* parent);
		virtual int			CompareFields		(BField* field1,
												 BField* field2);
};


//=====================================================================
// Field and column classes for bitmaps

class BBitmapField : public BField
{
	public:
							BBitmapField		(BBitmap* bitmap);
		const BBitmap*		Bitmap				();
		void				SetBitmap			(BBitmap* bitmap);

	private:
		BBitmap*			fBitmap;
};


//--------------------------------------------------------------------

class BBitmapColumn : public BTitledColumn
{
	public:
							BBitmapColumn		(const char* title,
												 float width,
												 float minWidth,
												 float maxWidth,
												 alignment align = B_ALIGN_LEFT);
		virtual void		DrawField			(BField*field,
												 BRect rect,
												 BView* parent);
		virtual int			CompareFields		(BField* field1, BField* field2);
		virtual	bool		AcceptsField        (const BField* field) const;
};
	

//=====================================================================
// Column to display BIntegerField objects as a graph.

class GraphColumn : public BIntegerColumn
{
	public:
							GraphColumn			(const char* name,
												 float width,
												 float minWidth,
												 float maxWidth,
												 alignment align = B_ALIGN_LEFT);
		virtual void		DrawField			(BField*field,
												 BRect rect,
												 BView* parent);
};

//HUGH
// column for displaying an icon next to a string, very useful
// this is realised as a combination of BString- and BBitmapColumn,
// which are handled internally by the object
class BBitmapStringColumn : public BTitledColumn
{
	public:
							BBitmapStringColumn	(const char* title,
												 float width,
												 float minWidth,
												 float maxWidth,
												 uint32 truncate,
												 alignment align = B_ALIGN_LEFT,
										 		 // spacing between bitmap and text
												 float spacing = 6.0);
		virtual void		DrawField			(BField* field,
												 BRect   rect,
												 BView*  parent);
		float				Spacing();			// return spacing bitmap<-->text
		void				SetSpacing(float spacing);
		
	private:
		BBitmapColumn*		fBitmapColumn;
		BStringColumn* 		fStringColumn;
		float				fSpacing;
};

// the field is implemented directly as a container for a BBitmapField and BStringField
// DrawField() then calls the respective Draw()-routines of the sub-fields, with correct rects
class BBitmapStringField : public BField
{
	public:
							BBitmapStringField	(BBitmap* bitmap,
												 const char *string);
												 
		BBitmapField*		BitmapField			();		// returns the enclosed BitmapField and its methods
		void 				SetBitmapField		(BBitmapField* field);
		BStringField*		StringField			();		// same for the enclosed StringField
		void 				SetStringField		(BStringField* field);

	private:
		BBitmapField*		fBitmapField;
		BStringField*		fStringField;
};

//HUGH
// column for displaying any BView (e.g. TextControl, BCheckBox,...)
class BViewColumn : public BTitledColumn
{
	public:
							BViewColumn (const char* title,
										 float width,
										 float minWidth,
										 float maxWidth,
										 alignment align = B_ALIGN_LEFT);
		virtual void		DrawField	(BField* field,
										 BRect   rect,
										 BView*  parent);
};

// the field holding the view
class BViewField : public BField
{
	public:
							BViewField (BView* view);
		// return the view encapsulated in the column
		BView*				View       ();
		void				SetView    (BView *view);
		
	private:
		BView*				fView;
};

#endif

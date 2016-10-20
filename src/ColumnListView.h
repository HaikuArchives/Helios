/*******************************************************************************
/
/	File:			ColumnListView.h
/
/   Description:    Experimental multi-column list view.
/
/	Copyright 2000+, Be Incorporated, All Rights Reserved
/
*******************************************************************************/

/*HUGH TODO
	FEATURES
	* respect new individual ColumnFlags on Resize/(Re)move...
	* better cursors (more slim, grey when resizing/moving is not possible)
	* hint when field is editable (draw outline)
	* allow edit-views that are bigger than the field (for calendar-popups etc.)
	* send configurable messages on events like SetVisible(), move, resize, add, remove
	* separate field-LOOK from field-FEEL (like BWindows) to allow e.g. for
	  B_MULTILINE_FEEL but B_SINGLE_LINE_LOOK (aka text-fields that expand to show multiple lines
	  if necessary - could be useful for OpenTracker later....)
	BUGS
	# navigation-bugs - getting better
	- display-bugs
	- color-banding not always correct, esp. when rows are collapsed/expanded (special condition)
	CODE
	- Get..Rect() are called FindRect(..) in OutlineView - should use same names, probably
	# remove dupliacte/dead code and clean up existant
	# tune performance
*/

#ifndef _COLUMN_LIST_VIEW_H
#define _COLUMN_LIST_VIEW_H

#include <BeBuild.h>
#include <View.h>
#include <List.h>
#include <Invoker.h>
#include <ListView.h>

#define kTEXT_MARGIN 8 // HUGH moved here from ColumnTypes.cpp (needed for editing-support)

// MOSCHT
#define FINISHED_EDITING_MSG 'C001'

class BScrollBar;

namespace BPrivate
{

class OutlineView;
class TitleView;
class BRowContainer;
class RecursiveOutlineIterator;

} // ns BPrivate

class BField;
class BRow;
class BColumn;
class BColumnListView;

enum LatchType { B_NO_LATCH, B_OPEN_LATCH, B_PRESSED_LATCH, B_CLOSED_LATCH };

// HUGH
enum LatchStyle {
	B_TRIANGLE_LATCH,  // right/down-triangle (closed/open) as used in most BeOS-apps, BeIDE, and on
					   // Macs
	B_PLUS_MINUS_LATCH // plus/minus-box as used in e.g. Windows
};					   //

typedef enum {
	B_ALLOW_COLUMN_NONE = 0,
	B_ALLOW_COLUMN_MOVE = 1,
	B_ALLOW_COLUMN_RESIZE = 2,
	B_ALLOW_COLUMN_POPUP = 4,
	B_ALLOW_COLUMN_REMOVE = 8,
	B_ALLOW_COLUMN_EDIT =
		16 // HUGH uses dynamic View (provided by column) for editing - see OutlineView::MouseDown()
} column_flags;

// HUGH special edit-flags
enum EditFlags {
	B_EDIT_MULTI_LINE = 0, // for multiline fields (= how to handle Enter)
	B_EDIT_EXIT_RETURN = 1 // exit on return? (if single-line-mode)
	// it is valid to set both of the flags above, in that case the user has to escape RETURN with
	// a qualifer /?? to be determined...
};
//-HUGH

enum ColumnListViewColor {
	B_COLOR_BACKGROUND = 0,
	B_COLOR_TEXT,
	B_COLOR_ROW_DIVIDER,
	B_COLOR_SELECTION,
	B_COLOR_SELECTION_TEXT,
	B_COLOR_NON_FOCUS_SELECTION,
	B_COLOR_EDIT_BACKGROUND,
	B_COLOR_EDIT_TEXT,
	B_COLOR_HEADER_BACKGROUND,
	B_COLOR_HEADER_TEXT,
	B_COLOR_SEPARATOR_LINE,
	B_COLOR_SEPARATOR_BORDER,
	// HUGH color for guides that indicate the hierarchy
	B_COLOR_GUIDES,
	// HUGH the following colors are used for every 1st, 2nd, 3rd,... row if
	//     ColorBanding is set (SetColorBands())
	//     this makes long/complex lists easier to read and also looks more professional
	B_COLOR_ROWS_1,
	B_COLOR_ROWS_2,
	B_COLOR_ROWS_3,
	B_COLOR_ROWS_4,
	B_COLOR_ROWS_5,
	B_COLOR_ROWS_6,
	B_COLOR_ROWS_7,
	B_COLOR_ROWS_8,
	// total amount of colors == boundary of color-array
	B_COLOR_TOTAL
};

enum ColumnListViewFont {
	B_FONT_ROW = 0,
	B_FONT_HEADER = 1,

	B_FONT_TOTAL = 2
};

// A single row/column intersection in the list.
class BField
{
public:
	BField();
	virtual ~BField();

	// HUGH editing-support
	// each individual field can be editable or not, regardless of the Column it belongs to
	bool IsEditable() { return fEditable; }
	void SetEditable(bool edit) { fEditable = edit; }
	// called when user clicks in a field
	// augmented by inherited BFields:
	// if edit == true -> user starts to edit this field -> write data to edit in the field
	// if edit == false-> user finished editing of field -> read changed data from field and store
	// it
	bool EditMode() { return fEditMode; }
	void SetEditMode(bool edit) { fEditMode = edit; }
	BColumn* GetColumn() { return fColumn; }
	void SetColumn(BColumn* column) { fColumn = column; }
	BRow* GetRow() { return fRow; }
	void SetRow(BRow* row) { fRow = row; }

private:
	BColumn* fColumn; // Column the field belongs to (for editing)
	BRow* fRow;		  // Row    --""--
	bool fEditable;
	bool fEditMode;
};

// A single line in the list.  Each line contains a BField object
// for each column in the list, associated by their "logical field"
// index.  Hierarchies are formed by adding other BRow objects as
// a parent of a row, using the AddRow() function in BColumnListView().
class BRow
{
public:
	BRow(float height = 16.0);
	virtual ~BRow();
	virtual bool HasLatch() const;

	int32 CountFields() const;
	BField* GetField(int32 logicalFieldIndex);
	const BField* GetField(int32 logicalFieldIndex) const;
	void SetField(BField* field, int32 logicalFieldIndex);

	float Height() const;
	bool IsExpanded() const;

private:
	// Blows up into the debugger if the validation fails.
	void ValidateFields() const;
	void ValidateField(const BField* field, int32 logicalFieldIndex) const;

private:
	BList fFields;
	BPrivate::BRowContainer* fChildList;
	bool fIsExpanded;
	float fHeight;
	BRow* fNextSelected;
	BRow* fPrevSelected;
	BRow* fParent;
	BColumnListView* fList;

	friend class BColumnListView;
	friend class BPrivate::RecursiveOutlineIterator;
	friend class BPrivate::OutlineView;
};

// Information about a single column in the list.  A column knows
// how to display the BField objects that occur at its location in
// each of the list's rows.  See ColumnTypes.h for particular
// subclasses of BField and BColumn that handle common data types.
class BColumn
{
public:
	BColumn(float width, float minWidth, float maxWidth, alignment align = B_ALIGN_LEFT);
	virtual ~BColumn();

	float Width() const;
	void SetWidth(float width);
	float MinWidth() const;
	float MaxWidth() const;

	virtual void DrawTitle(BRect rect, BView* targetView);
	virtual void DrawField(BField* field, BRect rect, BView* targetView);
	virtual int CompareFields(BField* field1, BField* field2);

	virtual void MouseMoved(BColumnListView* parent, BRow* row, BField* field, BRect field_rect,
							BPoint point, uint32 buttons, int32 code);
	virtual void MouseDown(BColumnListView* parent, BRow* row, BField* field, BRect field_rect,
						   BPoint point, uint32 buttons);
	virtual void MouseUp(BColumnListView* parent, BRow* row, BField* field);

	virtual void GetColumnName(BString* into) const;
	//			void		SetColumnName(const char *name);	//?HUGH should also be there?

	// HUGH ColumnFlags are now configured per column, not for the entire ColumnListView
	int GetColumnFlags() { return fColumnFlags; }
	// !!always call this method to set column-flags, also inside the object,
	// !!or fWantesEvents won't get updated properly!
	void SetColumnFlags(int flags);
	// for convenience
	void AddFlags(int flags) { SetColumnFlags(fColumnFlags | flags); }
	void RemoveFlags(int flags) { SetColumnFlags(fColumnFlags | (~flags)); }
	// Editing
	int GetEditFlags() { return fEditFlags; }
	void SetEditFlags(int flags) { fEditFlags = flags; }
	// shorthand-method
	bool IsEditable() { return fColumnFlags & B_ALLOW_COLUMN_EDIT; }

	// methods for adding and removing a view for editing
	// these are overloaded by the column-types to present a suitable view for editing
	// e.g. the StringView presents a TextView, a graph-column could present a slider
	// this view is generated when a column is editable and the user clicks on a field
	// see BColumn::MouseDown()
	virtual void AddEditView(BView* view, BField* field, BRect rect);
	virtual void RemoveEditView(BField* field);

	// get ColumnListView the BColumn belongs to
	BColumnListView* GetMasterView() { return fList; }
	//-HUGH

	bool IsVisible() const;
	void SetVisible(bool);

	bool WantsEvents() const;
	void SetWantsEvents(bool);

	bool ShowHeading() const;
	void SetShowHeading(bool);

	alignment Alignment() const;
	void SetAlignment(alignment);

	int32 LogicalFieldNum() const;

	/*!
		\param field The BField derivative to validate.

			Implement this function on your BColumn derivatives to validate BField derivatives
			that your BColumn will be drawing/manipulating.

			This function will be called when BFields are added to the Column, use dynamic_cast<> to
			determine if it is of a kind that your BColumn know how to handle. return false if it is
	   not.

			\note The debugger will be called if you return false from here with information about
				  what type of BField and BColumn and the logical field index where it occured.

			\note Do not call the inherited version of this, it just returns true;
	  */
	virtual bool AcceptsField(const BField* field) const;

private:
	float fWidth;
	float fMinWidth;
	float fMaxWidth;
	bool fVisible;
	int32 fFieldID;
	BColumnListView* fList;
	bool fSortAscending;
	bool fShowHeading;
	alignment fAlignment;
	int fColumnFlags; // HUGH
	int fEditFlags; // HUGH
	bool fWantsEvents;
	friend class BPrivate::OutlineView;
	friend class BColumnListView;
	friend class BPrivate::TitleView;
};

// The column list view class.
class BColumnListView : public BView, public BInvoker
{
public:
	BColumnListView(BRect rect, const char* name, uint32 resizingMode, uint32 drawFlags,
					border_style = B_NO_BORDER, bool showHorizontalScrollbar = true,
					uint32 latchStyle = B_TRIANGLE_LATCH);
	virtual ~BColumnListView();

	// Interaction
	virtual bool InitiateDrag(BPoint, bool wasSelected);
	virtual void MessageDropped(BMessage*, BPoint point);
	virtual void ExpandOrCollapse(BRow* row, bool expand);
	virtual status_t Invoke(BMessage* message = NULL);
	virtual void ItemInvoked();
	virtual void SetInvocationMessage(BMessage*);
	BMessage* InvocationMessage() const;
	uint32 InvocationCommand() const;
	BRow* FocusRow() const;
	void SetFocusRow(int32 index, bool select = false);
	void SetFocusRow(BRow* row, bool select = false);
	void SetMouseTrackingEnabled(bool);

	// Selection
	list_view_type SelectionMode() const;
	void Deselect(BRow* row);
	void AddToSelection(BRow* row);
	void DeselectAll();
	BRow* CurrentSelection(BRow* lastSelected = 0) const;
	virtual void SelectionChanged();
	virtual void SetSelectionMessage(BMessage*);
	BMessage* SelectionMessage();
	uint32 SelectionCommand() const;
	void SetSelectionMode(list_view_type); // list_view_type is defined in ListView.h.

	// Sorting
	void SetSortingEnabled(bool);
	bool SortingEnabled() const;
	void SetSortColumn(BColumn* column, bool add, bool ascending);
	void ClearSortColumns();

	// The status view is a little area in the lower left hand corner.
	void AddStatusView(BView* view);
	BView* RemoveStatusView();

	// Column Manipulation
	void AddColumn(BColumn*, int32 logicalFieldIndex);
	void MoveColumn(BColumn*, int32 index);
	void RemoveColumn(BColumn*);
	int32 CountColumns(bool countVisible = false) const;
	BColumn* ColumnAt(int32 index) const;
	BColumn* CurrentColumn(); // HUGH
	bool GetColumnRect(const BColumn* column, BRect* outRect) const; // HUGH
	int32 IndexOf(BColumn* column) { return fColumns.IndexOf(column); } // HUGH
	void SetColumnVisible(BColumn*, bool isVisible);
	void SetColumnVisible(int32, bool);
	bool IsColumnVisible(int32) const;
	void SetColumnFlags(column_flags flags); // HUGH obsolete, specific to each BColumn

	// Row manipulation
	const BRow* RowAt(int32 index, BRow* parent = 0) const;
	BRow* RowAt(int32 index, BRow* parent = 0);
	const BRow* RowAt(BPoint) const;
	BRow* RowAt(BPoint);
	BRow* CurrentRow(); // HUGH
	bool GetRowRect(const BRow* row, BRect* outRect) const;
	bool FindParent(BRow* row, BRow** outs_parent, bool* out_isVisible) const;
	int32 IndexOf(BRow* row);
	int32 CountRows(BRow* parent = 0) const;
	void AddRow(BRow*, BRow* parent = 0);
	void AddRow(BRow*, int32 index, BRow* parent = 0);

	void ScrollTo(const BRow* Row, bool scrollByPage = false);

	// Does not delete row or children at this time.
	// todo: Make delete row and children
	void RemoveRow(BRow*);

	void UpdateRow(BRow*);
	void Clear();

	// Appearance (DEPRECATED)
	void GetFont(BFont* font) const { BView::GetFont(font); }
	virtual void SetFont(const BFont* font, uint32 mask = B_FONT_ALL);
	virtual void SetHighColor(rgb_color);
	void SetSelectionColor(rgb_color);
	void SetBackgroundColor(rgb_color);
	void SetEditColor(rgb_color);
	const rgb_color SelectionColor() const;
	const rgb_color BackgroundColor() const;
	const rgb_color EditColor() const;

	// Appearance (NEW STYLE)
	void SetColor(int color_num, rgb_color color); // HUGH was ColumnListViewColor == too unflexible
	rgb_color Color(int color_num) const; // HUGH ditto
	void SetColorBands(int bands); // HUGH set number of bands used for color-banding of rows (max
								   // 8)
	int GetColorBands(); // HUGH return that number
	void SetFont(ColumnListViewFont font_num, const BFont* font, uint32 mask = B_FONT_ALL);
	void GetFont(ColumnListViewFont font_num, BFont* font) const;

	BPoint SuggestTextPosition(const BRow* row, const BColumn* column = NULL) const;

	void SetLatchWidth(float width);
	float LatchWidth() const;
	void SetLatchStyle(uint32 style); // HUGH set and get latch-style
	uint32 LatchStyle() const; // HUGH (currently arrow- or plusminus-style)
	virtual void DrawLatch(BView*, BRect, LatchType, BRow*);
	virtual void MakeFocus(bool isfocus = true);
	void SaveState(BMessage* msg);
	void LoadState(BMessage* msg);

	BView* ScrollView() const { return (BView*)fOutlineView; }
	// HUGH
	// Editing
	void SetEditMode(bool state); //		{ fEditMode = state;  }
	// better to access private member through public methods
	bool GetEditMode() { return fEditMode; }
	void SetEditField(BField* field) { fEditField = field; }
	BField* GetEditField() { return fEditField; }
	void SetEditView(BView* view) { fEditView = view; }
	BView* GetEditView() { return fEditView; }

	/*! EditNavigate controls editing-navigation
		\param delta_x <0 edit field to the left, >0 edit field to the right
		\param delta_y ditto for editing field above/below the current one
	*/
	void EditNavigate(int delta_x, int delta_y);
	void BeginEditing(BPoint point);
	void BeginEditing(BField* field, BRect fieldRect);
	void EndEditing();
	// Field manipulation
	// finds field at position -> returns field (+column) and updates Rect in place, NULL if not
	// found
	// optionally also returns Row with indent and top-offset
	BField* FindField(BPoint position, BRect* rect, BRow** outRow = NULL, int32* rowIndent = NULL,
					  float* rowTop = NULL);

	// returns field and field's Rect of field at row/column
	// optionally also returns Row with indent and top-offset
	BField* FindField(BRow* row, BColumn* column, BRect* rect, int32* rowIndent = NULL,
					  float* rowTop = NULL);
	BField* CurrentField();
	//-HUGH
	void Refresh();

	thread_id GetWaitingThreadID() { return fWaitingThreadID; };

protected:
	virtual void MessageReceived(BMessage* message);
	virtual void KeyDown(const char* bytes, int32 numBytes);
	virtual void AttachedToWindow();
	virtual void WindowActivated(bool active);
	virtual void Draw(BRect);

private:
	rgb_color fColorList[B_COLOR_TOTAL];
	// HUGH
	int fColorBands;	// >0 if different colors for odd/even/3rd/.. rows are to be used
	bool fEditMode;		// moved here from Outline/Title-View, now a *single* var!
	BField* fEditField; // field that's being edited
	BView* fEditView;   // dynamically inserted view for editable fields
	//-HUGH
	BPrivate::TitleView* fTitleView;
	BPrivate::OutlineView* fOutlineView;
	BList fColumns;
	BScrollBar* fHorizontalScrollBar;
	BScrollBar* fVerticalScrollBar;
	BList fSortColumns;
	BView* fStatusView;
	BMessage* fSelectionMessage;
	bool fSortingEnabled;
	float fLatchWidth;
	uint32 fLatchStyle;
	border_style fBorderStyle;
	thread_id fWaitingThreadID;
	bool fShowGuides; // HUGH draws lines to indicate the hierarchy
	// HUGH if we need more flags for the CLV, we should introduce a bitfield eventually
};

#endif

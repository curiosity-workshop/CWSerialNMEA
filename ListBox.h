#pragma once

#define LISTBOX_ITEM_HEIGHT 12
#define IN_RECT(x, y, l, t, r, b)	\
	(((x) >= (l)) && ((x) <= (r)) && ((y) >= (b)) && ((y) <= (t)))


#include <vector>
#include <string>


// This structure represents a listbox internally...it consists of arrays
	 // per item and some general stuff.
struct	XPListBoxData_t 
{
	// Per item:
	std::vector<std::string>	Items;		// The name of the item
	std::vector<int>			Lefts;		// The rectangle of the item, relative to the top left corner of the listbox/
	std::vector<int>			Rights;
	std::vector<int>			Reference;	// arbitrary reference passed in and out to caller
};

int listBoxGetItemNumber(XPListBoxData_t *inListBoxData , int inX, int inY);

static int		XPListBoxProc(
	XPWidgetMessage			inMessage,
	XPWidgetID				inWidget,
	intptr_t				inParam1,
	intptr_t				inParam2);

class ListBox
{
private:

	int _inLeft;
	int _inTop;
	int _inRight;
	int _inBottom;

	int _editMode;

	XPWidgetID _listBoxHandle;
	XPWidgetID _parentWidget;
	XPListBoxData_t* _pListBoxData;
	void _updateMetrics(void);

	static XPWidgetID           _XPCreateListBox(
		int                  inLeft,
		int                  inTop,
		int                  inRight,
		int                  inBottom,
		int                  inVisible,
		const char* inDescriptor,
		XPWidgetID           inContainer);

	
	
public:
	
	ListBox(int left, int right, int top, int bottom, XPWidgetID parentWidget, int editMode);
	~ListBox();

	void create(void);
	
	void clearItems(void);
	void addItem(const char* pBuffer, int reference);
	void insertItem(char* pBuffer, int Width, int CurrentItem, int reference);
	void deleteItem(int CurrentItem);
	int getCurrentItemReference(void);
	int getItemNumber(int inX, int inY);
};



enum {
	// This message is sent when an item is picked.
	// param 1 is the widget that was picked, param 2
	// is the item number.
	xpMessage_ListBoxItemSelected = 1900
};




enum {
	// This is the item number of the current item, starting at 0.
	xpProperty_ListBoxCurrentItem = 1900,
	// This will add an item to the list box at the end.
	xpProperty_ListBoxAddItem = 1901,
	// This will clear the list box and then add the items.
	xpProperty_ListBoxAddItemsWithClear = 1902,
	// This will clear the list box.
//	xpProperty_ListBoxClear = 1903,
	// This will insert an item into the list box at the index.
	xpProperty_ListBoxInsertItem = 1904,
	// This will delete an item from the list box at the index.
	xpProperty_ListBoxDeleteItem = 1905,
	// This stores the pointer to the listbox data.
	xpProperty_ListBoxData = 1906,
	// This stores the max Listbox Items.
	xpProperty_ListBoxMaxListBoxItems = 1907,
	// This stores the highlight state.
	xpProperty_ListBoxHighlighted = 1908,
	// This stores the scrollbar Min.
	xpProperty_ListBoxScrollBarMin = 1909,
	// This stores the scrollbar Max.
	xpProperty_ListBoxScrollBarMax = 1910,
	// This stores the scrollbar SliderPosition.
	xpProperty_ListBoxScrollBarSliderPosition = 1911,
	// This stores the scrollbar ScrollBarPageAmount.
	xpProperty_ListBoxScrollBarPageAmount = 1912
};

// Enums for x-plane native colors. 
enum {

	xpColor_MenuDarkTinge = 0,
	xpColor_MenuBkgnd,
	xpColor_MenuHilite,
	xpColor_MenuLiteTinge,
	xpColor_MenuText,
	xpColor_MenuTextDisabled,
	xpColor_SubTitleText,
	xpColor_TabFront,
	xpColor_TabBack,
	xpColor_CaptionText,
	xpColor_ListText,
	xpColor_GlassText,
	xpColor_Count
};

// Enums for the datarefs we get them from.
static const char* kXPlaneColorNames[] = {
	"sim/graphics/colors/menu_dark_rgb",
	"sim/graphics/colors/menu_bkgnd_rgb",
	"sim/graphics/colors/menu_hilite_rgb",
	"sim/graphics/colors/menu_lite_rgb",
	"sim/graphics/colors/menu_text_rgb",
	"sim/graphics/colors/menu_text_disabled_rgb",
	"sim/graphics/colors/subtitle_text_rgb",
	"sim/graphics/colors/tab_front_rgb",
	"sim/graphics/colors/tab_back_rgb",
	"sim/graphics/colors/caption_text_rgb",
	"sim/graphics/colors/list_text_rgb",
	"sim/graphics/colors/glass_text_rgb"
};

// Those datarefs are only XP7; if we can't find one,
// fall back to this table of X-Plane 6 colors.
static const float	kBackupColors[xpColor_Count][3] =
{
	 { (const float)(33.0 / 256.0), (const float)(41.0 / 256.0), (const float)(44.0 / 256.0) },
	 { (const float)(53.0 / 256.0), (const float)(64.0 / 256.0), (const float)(68.0 / 256.0) },
	 { (const float)(65.0 / 256.0), (const float)(83.0 / 256.0), (const float)(89.0 / 256.0) },
	 { (const float)(65.0 / 256.0), (const float)(83.0 / 256.0), (const float)(89.0 / 256.0) },
	 { (const float)0.8, (const float)0.8, (const float)0.8 },
	 { (const float)0.4, (const float)0.4, (const float)0.4 }
};

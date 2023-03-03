
#if IBM
#include <windows.h>
#endif
#if LIN
#include <GL/gl.h>
#else
#if __GNUC__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif
#endif


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>

#define XPLM200

#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMProcessing.h"
#include "XPLMDataAccess.h"
#include "XPLMMenus.h"
#include "XPLMUtilities.h"
#include "XPWidgets.h"
#include "XPStandardWidgets.h"
#include "XPLMCamera.h"
#include "XPUIGraphics.h"
#include "XPWidgetUtils.h"

#include "Config.h"
#include "XPLDevice.h"
#include "boardClass.h"
#include "XPLDirectCommon.h"
#include "ListBox.h"
#include "pinConfigDialog.h"




static XPWidgetID pinConfigWidgetRoot, pinConfigWindow;
ListBox* pinConfigListBox;
static XPWidgetID listBoxEditButton, listBoxCancelButton;
static XPWidgetID listBoxCaption1;
extern Config XPLConfig;
//static XPLMMenuID	id;

extern FILE* errlog;

extern XPLDevice* myXPLDevices[XPLDEVICES_MAXDEVICES];
extern boardClass boards;


void pinConfigDialogCreate(int deviceReference, int pinReference)
{


	createPinConfigWidgets(deviceReference, pinReference);
	return;
}


int pinConfigDialogActive(void)
{
	if (!XPIsWidgetVisible(pinConfigWidgetRoot)) return 0;
	else return 1;


}

/*------------------------------------------------------------------------*/

// This creates the widgets dialog and any controls
void createPinConfigWidgets(int deviceReference, int pinReference)
{
	//char entryName[100];
	int x;
	int y;
	int w;
	int h;
	int x2;
	int y2;
	int i;
	const char* title;

	XPLConfig.getWindowMetrics("XPLDirect.pinConfigDialogWindow", &title, &x, &y, &w, &h);
	x2 = x + w;
	y2 = y - h;

	

	pinConfigWidgetRoot = XPCreateWidget(x, y, x2, y2,		// root window
		1,	// Visible
		"Pin Editor",	// desc
		1,		// root
		NULL,	// no container
		xpWidgetClass_MainWindow);

	XPSetWidgetProperty(pinConfigWidgetRoot, xpProperty_MainWindowHasCloseBoxes, 1);

	pinConfigWindow = XPCreateWidget(x + 10, y - 30, x2 - 10, y2 + 20,
		1,	// Visible
		"",	// desc
		0,		// root
		pinConfigWidgetRoot,
		xpWidgetClass_SubWindow);

	XPSetWidgetProperty(pinConfigWindow, xpProperty_SubWindowType, xpSubWindowStyle_SubWindow);

	pinConfigListBox = new ListBox(x + 50, y - 50, x2 - 50, y - 300, pinConfigWidgetRoot);

	int pinCount = boards.getPinCount(myXPLDevices[deviceReference]->boardType);
	int pinIndex;
	int assignsCount;
	const char* assigns;
	const char* pinName;
	char pinNameTotal[80];

	fprintf(errlog, " pinConfigWindow is loading deviceReference %i,  %i pin descriptions from board type %i\n", deviceReference, pinCount, myXPLDevices[deviceReference]->boardType);

	for (i = 0; i < pinCount; i++)
	{
		if (boards.getPinInfo(myXPLDevices[deviceReference]->boardType, i, &pinIndex, &pinName, &assignsCount))
		{
			strcpy(pinNameTotal, pinName);
			strcat(pinNameTotal, ": ");
			for (int j = 0; j < assignsCount; j++)
			{
				strcat(pinNameTotal, "(");
				boards.getAssignInfo(myXPLDevices[deviceReference]->boardType, i, j, &assigns);
				strcat(pinNameTotal, assigns);
				strcat(pinNameTotal, ")");
			}

			if (assignsCount == 0)  strcat(pinNameTotal, "Not Assignable");

			//	fprintf(errlog, "   createPinConfigWidgets:  found pin %i, index %i, assigns %i, name %s\n", i, pinIndex, assignsCount, pinNameTotal);
			pinConfigListBox->addItem(pinNameTotal, pinIndex);
		}

	}

	pinConfigListBox->create();

	listBoxEditButton = XPCreateWidget(x + 70, y - 350, x + 130, y - 375,
		1, "Edit", 0, pinConfigWidgetRoot,
		xpWidgetClass_Button);

	XPSetWidgetProperty(listBoxEditButton, xpProperty_ButtonType, xpPushButton);

	listBoxCancelButton = XPCreateWidget(x + 160, y - 350, x + 210, y - 375,
		1, "Cancel", 0, pinConfigWidgetRoot,
		xpWidgetClass_Button);

	XPSetWidgetProperty(listBoxCancelButton, xpProperty_ButtonType, xpPushButton);

	listBoxCaption1 = XPCreateWidget(x + 100, y - 300, x + 210, y - 350,
		1, "Select pin to Configure", 0, pinConfigWidgetRoot,
		xpWidgetClass_Caption);

	XPAddWidgetCallback(pinConfigWidgetRoot, pinConfigWidgetsHandler);

}

/*------------------------------------------------------------------------*/

// Handle any widget messages
int	pinConfigWidgetsHandler(
	XPWidgetMessage			inMessage,
	XPWidgetID				inWidget,
	intptr_t				inParam1,
	intptr_t				inParam2)
{
	//char Buffer[256];

	// Close button pressed, only hide the widget, rather than destropying it.
	if (inMessage == xpMessage_CloseButtonPushed)
	{

		XPDestroyWidget(pinConfigWidgetRoot, 1);  // and your little dog too
		return 1;
	}

	// Test for a button pressed
	if (inMessage == xpMsg_PushButtonPressed)
	{

		// Cancel button selected
		if (inParam1 == (intptr_t)listBoxCancelButton)
		{

			XPDestroyWidget(pinConfigWidgetRoot, 1);  // and your little dog too
			return 1;

		}

		// Edit button selected
		if (inParam1 == (intptr_t)listBoxEditButton)
		{
			fprintf(errlog, "pinConfigWindow:  User selected to edit item %i, \n", pinConfigListBox->getCurrentItemReference());
			//if (!pinConfigWindowActive()) pinConfigWindowCreate();
			return 1;
		}


	}
	// This handles the listbox item selected message
	// i.e. when you click on an item
	if (inMessage == xpMessage_ListBoxItemSelected)
	{
		//		XPGetWidgetDescriptor(TestWidgetsListBox, Buffer, sizeof(Buffer));
		//		XPSetWidgetDescriptor(ListboxInfoText1, Buffer);
		//		sprintf(Buffer, "ListBox Index %d", inParam2);
		//		XPSetWidgetDescriptor(ListboxInfoText2, Buffer);
		return 1;
	}


	return 0;
}

/*------------------------------------------------------------------------*/


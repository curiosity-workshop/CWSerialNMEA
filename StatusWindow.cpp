
#include <stdio.h>
#include <string>
#include <ctime>  



#define XPLM200


#include "XPLMPlugin.h"
#include "XPLMDataAccess.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMMenus.h"

#include "XPWidgets.h"
#include "XPStandardWidgets.h"

#include "XPLMUtilities.h"
#include "XPLMProcessing.h"

#include "XPLMCamera.h"
#include "XPUIGraphics.h"
#include "XPWidgetUtils.h"

//#include "configDescriptions.h"
#include "serialclass.h"
#include "Config.h"

#include "CWSerial.h"

#include "DataHandler.h"
#include "StatusWindow.h"


extern DataHandler *CWDataHandler;

XPLMWindowID	statusWindow = NULL;
//extern configDescriptions descriptions;

extern long int packetsSent;
extern long int packetsReceived;
extern long cycleCount;
extern float elapsedTime;
extern int refreshRate;


void statusWindowCreate()
{
	statusWindow = XPLMCreateWindow(
		50, 600, 800, 200,			/* Area of the window. */
		1,							/* Start visible. */
		statusDrawWindowCallback,		/* Callbacks */
		statusHandleKeyCallback,
		statusHandleMouseClickCallback,
		NULL);						/* Refcon - not used. */
}

/**************************************************************************************/
/* MyDrawWindowCallback -- Called by xplane while window is active        */
/**************************************************************************************/
void statusDrawWindowCallback(
	XPLMWindowID         inWindowID,
	void* inRefcon)
{
	int		left, top, right, bottom;
	float	color[] = { 1.0, 1.0, 1.0 }; 	/* RGB White */
	char tstring[800];

	/* First we get the location of the window passed in to us. */
	XPLMGetWindowGeometry(inWindowID, &left, &top, &right, &bottom);

	/* We now use an XPLMGraphics routine to draw a translucent dark
	 * rectangle that is our window's shape. */
	XPLMDrawTranslucentDarkBox(left, top, right, bottom);

	/* Finally we draw the text into the window, also using XPLMGraphics
	 * routines.  The NULL indicates no word wrapping. */
	XPLMDrawString(color, left + 5, top - 20, "Curiosity Workshop CWSerialNMEA Plugin.  Updates and Examples: www.patreon.com/curiosityworkshop", NULL, xplmFont_Basic);
	//XPLMDrawString(color, left + 5, top - 20, "Curiosity Workshop AgPilotX Connection Plugin.  Updates and Examples: www.patreon.com/curiosityworkshop", NULL, xplmFont_Basic);

	sprintf(tstring, "Distribution Build: %u.  Click this window when complete", CWSERIAL_VERSION);
	XPLMDrawString(color, left + 5, top - 35, tstring, NULL, xplmFont_Basic);


	sprintf(tstring, "Requested Com Port: com%i, Baud Rate: %i.  tx: %i, rx: %i",
		CWDataHandler->getComPort(), CWDataHandler->getBaudRate(), CWDataHandler->getPacketsSent(), CWDataHandler->getPacketsReceived());
	XPLMDrawString(color, left + 5, top - 60, tstring, NULL, xplmFont_Basic);
	
	sprintf(tstring, "Requested Refresh Rate in ms: %i", refreshRate);
	XPLMDrawString(color, left + 5, top - 75, tstring, NULL, xplmFont_Basic);
	
	sprintf(tstring, "Elapsed time since start: %i, cycles: %i, average time between cycles: %3.2f", (int)elapsedTime, cycleCount, elapsedTime / cycleCount);
	XPLMDrawString(color, left + 5, top - 90, tstring, NULL, xplmFont_Basic);

	sprintf(tstring, "Last Transmission: %s", CWDataHandler->transmitString);
	XPLMDrawString(color, left + 5, top - 120, tstring, NULL, xplmFont_Basic);

/*	for (int i = 0; i < validPorts; i++)
	{
		if (myXPLDevices[i])
		{
			if (strlen(myXPLDevices[i]->lastDebugMessageReceived))
				sprintf(tstring, "Last message device %s: %s", myXPLDevices[i]->deviceName, myXPLDevices[i]->lastDebugMessageReceived);
			else
			{
				if (myXPLDevices[i]->deviceType == XPLTYPE_XPLDIRECT)
					sprintf(tstring, "Device %i type XPLDirect: %s", i, myXPLDevices[i]->deviceName);
				else
					sprintf(tstring, "Device %i type XPLWizard: %s (%s)", i, myXPLDevices[i]->deviceName, descriptions.getBoardName(myXPLDevices[i]->boardType));
			}
			XPLMDrawString(color, left + 5, top - (80 + i * 10), tstring, NULL, xplmFont_Basic);
		}
	}
*/


}

int statusWindowActive(void)
{
	if (!statusWindow) return 0;
	else return 1;
}


/**************************************************************************************/
/* MyHandleMouseClickCallback -- Called by xplane while status window is active  */
/**************************************************************************************/
int statusHandleMouseClickCallback(
	XPLMWindowID         inWindowID,
	int                  x,
	int                  y,
	XPLMMouseStatus      inMouse,
	void* inRefcon)
{

	XPLMDestroyWindow(statusWindow);
	statusWindow = NULL;

	return 1;
}

/**************************************************************************************/
/* MyHandleKeyCallback -- Called by xplane while status window is active        */
/**************************************************************************************/
void statusHandleKeyCallback(
	XPLMWindowID         inWindowID,
	char                 inKey,
	XPLMKeyFlags         inFlags,
	char                 inVirtualKey,
	void* inRefcon,
	int                  losingFocus)
{
}


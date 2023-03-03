/*
  CWSerial - plugin for serial data flow from Xplane
  Created by Michael Gerlicher,  December 2022.

  To report problems, download updates and examples, suggest enhancements or get technical support, please visit my patreon page:

	 www.patreon.com/curiosityworkshop

	 
*/


/*
		Todo:  
		
	
*/

#include <stdio.h>


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




//#include "serialclass.h"

#include "DataHandler.h"
#include "Config.h"
#include "StatusWindow.h"
#include "CWSerial.h"

FILE* serialLogFile;			// for serial data log
FILE* errlog;				// Used for logging problems

Config *CWConfig;
DataHandler *CWDataHandler;




long cycleCount = 0l;
float elapsedTime = 0;
int logSerial = false;
int refreshRate = 10;			// how often in MS to send data

int				gClicked = 0;
XPLMMenuID      myMenu;
int             disengageMenuItemIndex;
int				logSerialMenuItemIndex;





PLUGIN_API int XPluginStart(
						char *		outName,
						char *		outSig,
						char *		outDesc)
{
		
		int			mySubMenuItem;
		
		char outFilePath[256];
	
	XPLMDebugString("CWSerialNMEA:  Initializing Plug-in\n");

    fopen_s(&errlog, "CWSerialNMEAError.log", "w");
	if (!errlog)
	{
		XPLMDebugString("CWSerialNMEA:  Unable to open error log file CWSerialNMEAError.log\n");
		return 0;
	}
	setbuf(errlog, NULL);

// Provide our plugin's profile to the plugin system. 
	strcpy(outName, "CWSerialNMEA");
	strcpy(outSig, "cwserial.rs232.cw");
	strcpy(outDesc, "NMEA Serial Data Output Plugin");


	fprintf(errlog, "Curiosity Workshop CWSerialNMEA version %u copyright 2023.  \n", CWSERIAL_VERSION );

	fprintf(errlog, "To report problems, download updates and examples, suggest enhancements or get technical support:\r\n");
	fprintf(errlog, "    patreon:  www.patreon.com/curiosityworkshop\n");
	fprintf(errlog, "    discord: https://discord.gg/RacvaRFsMW\n");
	fprintf(errlog, "    YouTube:  youtube.com/channel/UCISdHdJIundC-OSVAEPzQIQ \n\n");

	fprintf(errlog, "CWSerialNMEA plugin Error log file begins now.\r\n");
	
	XPLMGetPluginInfo( XPLMGetMyID(), NULL, outFilePath, NULL, NULL);   
	fprintf(errlog, "Plugin Path: %s\r\n", outFilePath);

	// first load configuration stuff
	CWConfig = new Config(CFG_FILE);
	logSerial = CWConfig->getSerialLogFlag();
	CWConfig->getRefreshRate(&refreshRate);

	CWDataHandler = new DataHandler();
	CWDataHandler->begin();

	

	if (logSerial) fprintf(errlog, "Serial logging enabled.\r\n");  else fprintf(errlog, "Serial logging disabled.\r\n");
	
	

	
	
	/* First we put a new menu item into the plugin menu.
	 * This menu item will contain a submenu for us. */
	mySubMenuItem = XPLMAppendMenuItem(
						XPLMFindPluginsMenu(),	/* Put in plugins menu */
						"CWSerialNMEA",				/* Item Title */
						//"AgPilotX Connection",
						0,						/* Item Ref */
						1);						/* Force English */
	
	/* Now create a submenu attached to our menu item. */
	myMenu = XPLMCreateMenu(
						"XPLDirectB", 
						XPLMFindPluginsMenu(), 
						mySubMenuItem, 			/* Menu Item to attach to. */
						MyMenuHandlerCallback,	/* The handler */
						0);						/* Handler Ref */
						
	/* Append a few menu items to our submenu.   */
	
	XPLMAppendMenuItem(myMenu, "Status",(void *) "Status", 1);

	logSerialMenuItemIndex = XPLMAppendMenuItem(myMenu, "Log Serial Data", (void*) "Log Serial Data", 1);
	XPLMAppendMenuSeparator(myMenu);

	if (logSerial) XPLMCheckMenuItem(myMenu, logSerialMenuItemIndex, xplm_Menu_Checked);
	else           XPLMCheckMenuItem(myMenu, logSerialMenuItemIndex, xplm_Menu_Unchecked);

	XPLMRegisterFlightLoopCallback(							// Setup timed processing		
		MyFlightLoopCallback,	/* Callback */
		-2.0,					/* Interval */
		NULL);					/* refcon not used. */

	

	if (logSerial)
	{
		fopen_s(&serialLogFile, "CWSerialNMEA.log", "w");
		if (serialLogFile) fprintf(serialLogFile, "Serial logger.  Unprintable characters are represented by '~'\n");
	}

	XPLMDebugString("CWSerialNMEA:  Plugin initialization complete.\n");


	
	return 1;
}


// XPluginStop

PLUGIN_API void	XPluginStop(void)
{
//	cmpSelect.saveConfiguration();
//	cmpLEDEdit.saveConfiguration();

//	if (errlog) fprintf(errlog, "Ending plugin, cycle count: %u Packets transmitted: %u\n", cycleCount, packetsSent);
	if (errlog) fclose(errlog);


	if (serialLogFile) fclose(serialLogFile);

	
}


// XPluginDisable Handler.  We do nothing in this plugin
PLUGIN_API void XPluginDisable(void)
{
}

//XpluginEnable handler.  We do nothing in this plugin.
PLUGIN_API int XPluginEnable(void)
{
	return 1;
}

// XPluginRecieveMessage Handler
PLUGIN_API void XPluginReceiveMessage(
	XPLMPluginID	inFromWho,
	int				inMessage,
	void* inParam)
{
	char pluginName[256];

	XPLMGetPluginInfo(inFromWho, pluginName, NULL, NULL, NULL);

	if (inMessage == XPLM_MSG_PLANE_UNLOADED)
	{
	

	}
	if (inMessage == 108) // 108 is supposed to = XPLM_MSG_LIVERY_LOADED
	//if (inMessage == XPLM_MSG_PLANE_LOADED)
	{
		
	}

//	fprintf(errlog, "Xplane sent me a message from: %s, message: %i\n", pluginName, inMessage);
}


/**************************************************************************************/
/* MyMenuHandlerCallback-- Handle users request for calibration                       */
/**************************************************************************************/
void	MyMenuHandlerCallback(
	void* inMenuRef,
	void* inItemRef)
{

//	fprintf(errlog, "User selected inMenuRef: %i, inItemRef: %i\n", (int)(int *)inMenuRef, (int)(int *)inItemRef);
	

	// Handle request for status window
	if (!strcmp((const char*)inItemRef, "Status"))   // menu 0, item 1, "Status"
	{
		if (!statusWindowActive() ) statusWindowCreate();
	}
	
	

	// Handle request toggle for serial logging
	if (!strcmp((const char*)inItemRef, "Log Serial Data"))   // menu 0, item 1, "Log Serial Data"
	{
		if (logSerial)
		{
			fclose(serialLogFile);
			logSerial = false;
		    XPLMCheckMenuItem(myMenu, logSerialMenuItemIndex, xplm_Menu_Unchecked);
			//CWConfig->setSerialLogFlag(0);
		}
		else
		{
			fopen_s(&serialLogFile, "CWSerialNMEA.log", "w");
			
			if (serialLogFile)
			{
				fprintf(serialLogFile, "Serial logger.  Unprintable characters are represented by '~'\n");
				logSerial = true;
				XPLMCheckMenuItem(myMenu, logSerialMenuItemIndex, xplm_Menu_Checked);
				//CWConfig->setSerialLogFlag(1);
			}

		}
	}


}




/**************************************************************************************/
/* MyFlightLoopCallback -- Called by xplane every few flight loops                    */
/**************************************************************************************/
float	MyFlightLoopCallback(
	float                inElapsedSinceLastCall,
	float                inElapsedTimeSinceLastFlightLoop,
	int                  inCounter,
	void* inRefcon)
{
	//return XPLDIRECT_RETURN_TIME;
	//fprintf(errlog, "Time:  %f \n", inElapsedSinceLastCall);
	elapsedTime += inElapsedSinceLastCall;

	//if (cycleCount == 1)
//	{
	//	engageDevices();
		//sendRefreshRequest();
//	}

    CWDataHandler->processSerial();

    cycleCount++;
	return (float)refreshRate / 1000;
}




int widgetMessageDispatcher(XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t	inParam1, intptr_t inParam2)
{
	// need to add for each dialog for now


	//if (cmpSelect.isYourWidget(inWidget))	return cmpSelect.widgetMessageHandler(inMessage, inWidget, inParam1, inParam2);
	//if (cmpLEDEdit.isYourWidget(inWidget))	return cmpLEDEdit.widgetMessageHandler(inMessage, inWidget, inParam1, inParam2);
	
	
	return 0;	// always return 0 if we don't know what to do with the message

}

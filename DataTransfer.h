#pragma once

#include "Serial.h"

void BindingsSetup(void);
void BindingsLoad(void);
bool getComPorts(void);
void activateDevices(void);
void sendRefreshRequest(void);
void disengageDevices(void);
void engageDevices(void);
void _processPacket(int);
void _processSerial(void);
void _updateDataRefs(int forceUpdate);
int _writePacket(int port, char, char*);
int _writePacketN(int port, char, char*, int);
void reloadDevices(void);

struct DataRefBinding
{
	int            deviceIndex;				// which XPLDirectDevice is this attached to
	int            bindingActive;			// Is this binding being used
	int            Handle;			        // Handle is arbitrary and incremental and assigned by this plugin to send to arduino board
	int            RWMode;					// XPL_READ 1   XPL_WRITE   2   XPL_READWRITE	3
	float		   divider;					// reduce resolution by dividing then remultiplying with this number, or 0 for no processing
	int            updateRate;				// minimum time in ms between updates sent 
	time_t		   lastUpdate;				// time of last update
	XPLMDataRef    xplaneDataRefHandle;		// Dataref handle of xplane element associated with binding
	XPLMDataTypeID xplaneDataRefTypeID;		// dataRef type
	char           xplaneDataRefName[80];		// character name of xplane dataref
	int			   xplaneDataRefArrayOffset;// if xplane data is array data, specify array element otherwise set to -1
	long           xplaneCurrentSentl;		// Current  long value sent to device
	long           xplaneCurrentReceivedl;   // Current long value sent to Xplane
	float          xplaneCurrentSentf;      // Current float value sent to device
		
	float          xplaneCurrentReceivedf;  // Current float value sent to Xplane
	double			xplaneCurrentSentD;		// current double value sent to device
	double			xplaneCurrentReceivedD;	// current double value sent to Xplane
	
	char* xplaneCurrentSents;	// dynamically allocated string buffer for string types.


};

struct CommandBinding
{
	int            deviceIndex;				// which XPLDirectDevice is this attached to
	int            bindingActive;			// Is this binding being used
	int            Handle;			        // Handle is incremental and assigned by this plugin to send to arduino board

	XPLMCommandRef xplaneCommandHandle;		// Dataref handle of xplane element associated with binding

	char           xplaneCommandName[80];		// character name of xplane dataref

	int            xplaneCurrentReceived;   // Current value sent to Xplane

};

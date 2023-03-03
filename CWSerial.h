
#pragma once

#define CWSERIAL_VERSION 23011801
#define XPLM200


#define CFG_FILE				"Resources\\plugins\\CWSerialNMEA\\CWSerialNMEA.cfg"

#define CWSERIAL_MILLIS_BETWEEN_FRAMES_DEFAULT 0			// for data sends
#define CWSERIAL_RETURN_TIME   .05							// request next visit every .05 seconds or - for cycles

#define CWSERIAL_TIMEOUT_SECONDS 3
#define CWSERIAL_PACKET_SIZE 1000 

#define MAX_DATAREFS 250

float	MyFlightLoopCallback(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter, void* inRefcon);
static void	MyMenuHandlerCallback(
    void* inMenuRef,
    void* inItemRef);


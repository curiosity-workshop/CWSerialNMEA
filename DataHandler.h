#pragma once
#include "CWSerial.h"
#include "Serial.h"

#define NMEAGxGGA 0
#define NMEAGxRMC 1


class DataHandler
{
private:
	
	// local globals
	int _validConfig;

	int _dataRefCount = 0;

	int _comPort;
	int _baudRate;
	int _packetsSent = 0;
	int _packetsReceived = 0;
	
	
	XPLMDataRef _latitudeHandle;
	XPLMDataRef _longitudeHandle;
	XPLMDataRef _elevationHandle;
	XPLMDataRef _groundspeedHandle;
	XPLMDataRef _zuluHoursHandle;
	XPLMDataRef _zuluMinutesHandle;
	XPLMDataRef _zuluSecondsHandle;
	XPLMDataRef _groundTrackHandle;
	XPLMDataRef _magVariationHandle;

	char _latitudeString[20];
	char _longitudeString[20];
	char _elevationString[20];
	char _groundspeedString[20];
	char _zuluTimeString[20];
	
	//struct 
//	{
	//	const char name[80];
	//}	datarefs[MAX_DATAREFS];

public:

    DataHandler();
    ~DataHandler();
	int begin(void);
	int getComPort();
	int getBaudRate();
	int getDataRefCount();
	int getPacketsSent();
	int getPacketsReceived();
	int getZuluTimeString(char*);

	int processSerial(void);
	
	int sendGNGGA(int zulu_time_hours, int zulu_time_minutes, int zulu_time_seconds, double latitude, char latitudeFlag, double longitude, char longitudeFlag,
		float elevation);
	int sendGNRMC(int zulu_time_hours, int zulu_time_minutes, int zulu_time_seconds, double latitude, char latitudeFlag, double longitude, char longitudeFlag,
		float groundSpeed, float groundTrack);

	int _loadDatarefs(void);
	int _processDatarefs(void);
	int _writePacket(int port, char, char*);
	int _writePacketN(int port, char, char*, int);
	char _getChecksum(char* inString);
	char transmitString[200];
	double _DDtoDM(double inValue);

	int _nextSentence;

	Serial* serialPort;

	time_t		   lastUpdate;				// time of last update

	char           xplaneDataRefName[80];		// character name of xplane dataref
	int			   xplaneDataRefArrayOffset;// if xplane data is array data, specify array element otherwise set to -1

	char* xplaneCurrentSents;	// dynamically allocated string buffer for string types.
};





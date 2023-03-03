
#define XPLM200

#include <math.h>

#include "Serial.h"

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


#include "CWSerial.h"
#include "Config.h"
#include "Serial.h"
#include "DataHandler.h"

#include <ctime>



long int packetsSent;
long int packetsReceived;

int validPorts = 0;

extern int logSerial;
extern FILE* errlog;
extern FILE* serialLogFile;
extern float elapsedTime;
extern Config* CWConfig;

DataHandler::DataHandler()
{
	

	
	
}

DataHandler::~DataHandler()
{
	delete serialPort;
}

int DataHandler::begin(void)
{

	char name[80];

	CWConfig->getSerialParameters(&_comPort, &_baudRate);
	fprintf(errlog, "DataHandler opening com port: com%i, baud rate: %i\n ", _comPort, _baudRate);

	serialPort = new Serial(_comPort, _baudRate);

	_loadDatarefs();
	_nextSentence = NMEAGxRMC;
	return 0;
}

int DataHandler::getComPort()
{
	
	return _comPort;
}

int DataHandler::getBaudRate()
{
	return _baudRate;
}

int DataHandler::getDataRefCount()
{
	return _dataRefCount;

}

int DataHandler::getPacketsSent()
{
	return _packetsSent;

}

int DataHandler::getPacketsReceived()
{
	return _packetsReceived;
}
int DataHandler::getZuluTimeString(char *inString)
{
	strcpy(inString, _zuluTimeString);
	
	return 0;

}

int DataHandler::processSerial(void)
{
	int zulu_time_hours;
	int zulu_time_minutes;
	int zulu_time_seconds;
	float elevation;

	float groundSpeed;
	float groundTrack;
	float magVariation;

	double latitude, latitudeDDM;
	double longitude, longitudeDDM;
	char latitudeFlag;
	char longitudeFlag;

	zulu_time_hours = XPLMGetDatai(_zuluHoursHandle);
	zulu_time_minutes = XPLMGetDatai(_zuluMinutesHandle);
	zulu_time_seconds = XPLMGetDatai(_zuluSecondsHandle);

	latitude = XPLMGetDataf(_latitudeHandle);
	if (latitude < 0)
	{
		latitudeFlag = 'S';
		latitude = -latitude;

	}
	else latitudeFlag = 'N';
	latitudeDDM = _DDtoDM(latitude);


	longitude = XPLMGetDataf(_longitudeHandle);
	if (longitude < 0)
	{
		longitudeFlag = 'W';
		longitude = -longitude;
	}
	else longitudeFlag = 'E';
	longitudeDDM = _DDtoDM(longitude);
	
	elevation = XPLMGetDataf(_elevationHandle);
	groundSpeed = XPLMGetDataf(_groundspeedHandle) *1.94384;  // convert meters/second to knots
	magVariation = XPLMGetDataf(_magVariationHandle);
	groundTrack = XPLMGetDataf(_groundTrackHandle) + magVariation;

	if (_nextSentence == NMEAGxGGA)
	{
		sendGNGGA(zulu_time_hours, zulu_time_minutes, zulu_time_seconds, latitudeDDM, latitudeFlag, longitudeDDM, longitudeFlag, elevation);
		_nextSentence = NMEAGxRMC;
		return 0;
	}

	if (_nextSentence == NMEAGxRMC)
	{
		sendGNRMC(zulu_time_hours, zulu_time_minutes, zulu_time_seconds, latitudeDDM, latitudeFlag, longitudeDDM, longitudeFlag, groundSpeed, groundTrack);
		_nextSentence = NMEAGxGGA;
		return 0;
	}

	return 0;
}

int DataHandler::sendGNGGA(int zulu_time_hours, int zulu_time_minutes, int zulu_time_seconds, double latitude, char latitudeFlag, double longitude,char longitudeFlag,
							float elevation)
{
	char checksumString[10];


	sprintf(transmitString, "$GPGGA,%2.2i%2.2i%2.2i.00,%lf,%c,%lf,%c,1,12,1.0,%6.2lf,M,1,M,,*",
		zulu_time_hours,
		zulu_time_minutes,
		zulu_time_seconds,
		latitude,
		latitudeFlag,
		longitude,
		longitudeFlag,
		elevation);

	char checksum = _getChecksum(transmitString);
	sprintf(checksumString, "%2X\n", checksum);
	strcat(transmitString, checksumString);

	if (serialPort->WriteData(transmitString, strlen(transmitString) ) == true)
	{
		_packetsSent++;
	}
	
	if (logSerial)  fprintf(serialLogFile, "%s", transmitString);

	return 0;
}

int DataHandler::sendGNRMC(int zulu_time_hours, int zulu_time_minutes, int zulu_time_seconds, double latitude, char latitudeFlag, double longitude, char longitudeFlag,
							float groundSpeed, float groundTrack)
{
	char checksumString[10];


	sprintf(transmitString, "$GPRMC,%2.2i%2.2i%2.2i.00,A,%lf,%c,%lf,%c,%1.1f,%3.1f,010122,,,A*",
		zulu_time_hours,
		zulu_time_minutes,
		zulu_time_seconds,
		latitude,
		latitudeFlag,
		longitude,
		longitudeFlag,
		groundSpeed,			// meters/sec to knots
		groundTrack );

	char checksum = _getChecksum(transmitString);
	sprintf(checksumString, "%2X\n", checksum);
	strcat(transmitString, checksumString);

	serialPort->WriteData(transmitString, strlen(transmitString));
	_packetsSent++;
	if (logSerial)  fprintf(serialLogFile, "%s", transmitString);

	return 0;
}

char DataHandler::_getChecksum(char* inString)
{
	int i = 1;
	char checksum = 0;

	while (inString[i] != '*' && i < 200)
	{
		checksum ^= inString[i];
		i++;
	}
			
	return checksum;
}


int DataHandler::_loadDatarefs(void)
{

	_latitudeHandle = XPLMFindDataRef("sim/flightmodel/position/latitude");
	_longitudeHandle = XPLMFindDataRef("sim/flightmodel/position/longitude");
	_elevationHandle = XPLMFindDataRef("sim/flightmodel/position/elevation");
	_groundspeedHandle = XPLMFindDataRef("sim/flightmodel/position/groundspeed");
	_zuluHoursHandle = XPLMFindDataRef("sim/cockpit2/clock_timer/zulu_time_hours");
	_zuluMinutesHandle = XPLMFindDataRef("sim/cockpit2/clock_timer/zulu_time_minutes");
	_zuluSecondsHandle = XPLMFindDataRef("sim/cockpit2/clock_timer/zulu_time_seconds");
	_groundTrackHandle = XPLMFindDataRef("sim/cockpit2/gauges/indicators/ground_track_mag_pilot");
	_magVariationHandle = XPLMFindDataRef("sim/flightmodel/position/magnetic_variation");

	return 0;
}

int DataHandler::_processDatarefs(void)
{

	

	return 0;
}


double DataHandler::_DDtoDM(double inValue)
{
	double degrees;
	double minutes;

	//degrees = floor(inValue);
	minutes = modf(inValue, &degrees);
	

	//fprintf(errlog, "degrees: %f: minutes: %f  conversion: %f\r\n", degrees, minutes, degrees*100 + minutes*60);

	return (degrees * 100) + (minutes*60);



}

/*
To understand the NMEA message structure, let’s examine the popular $GPGGA message.This particular message was output from an RTK GPS receiver :

$GPGGA, 181908.00, 3404.7041778, N, 07044.3966270,
W, 4, 13, 1.00, 495.144, M, 29.200, M, 0.10, 0000 * 40

All NMEA messages start with the $ character, and each data field is separated by a comma.

GP represent that it is a GPS position(GL would denote GLONASS).

181908.00 is the time stamp : UTC time in hours, minutesand seconds.

3404.7041778 is the latitude in the DDMM.MMMMM format.Decimal places are variable.

N denotes north latitude.

07044.3966270 is the longitude in the DDDMM.MMMMM format.Decimal places are variable.

W denotes west longitude.

4 denotes the Quality Indicator :

1 = Uncorrected coordinate

2 = Differentially correct coordinate(e.g., WAAS, DGPS)

4 = RTK Fix coordinate(centimeter precision)

5 = RTK Float(decimeter precision.

	13 denotes number of satellites used in the coordinate.

	1.0 denotes the HDOP(horizontal dilution of precision).

	495.144 denotes altitude of the antenna.

	M denotes units of altitude(eg.Meters or Feet)

	29.200 denotes the geoidal separation(subtract this from the altitude of the antenna to arrive at the Height Above Ellipsoid(HAE).

		M denotes the units used by the geoidal separation.

		1.0 denotes the age of the correction(if any).

		0000 denotes the correction station ID(if any).

		* 40 denotes the checksum.

		The $GPGGA is a basic GPS NMEA message.There are alternative and companion NMEA messages that provide similar or additional information.

		Here are a couple of popular NMEA messages similar to the $GPGGA message with GPS coordinates in them(these can possibly be used as an alternative to the $GPGGA message) :

		$GPGLL, $GPRMC

		In addition to NMEA messages that contain a GPS coordinate, several companion NMEA messages offer additional information besides the GPS coordinate.Following are some of the common ones :

$GPGSA – Detailed GPS DOPand detailed satellite tracking information(eg.individual satellite numbers).$GNGSA for GNSS receivers.

$GPGSV – Detailed GPS satellite information such as azimuthand elevation of each satellite being tracked.$GNGSV for GNSS receivers.

$GPVTG – Speed over groundand tracking offset.

$GPGST – Estimated horizontaland vertical precision.$GNGST for GNSS receivers.

Rarely does the $GPGGA message have enough information by itself.For example, the following screen requires: $GPGGA, $GPGSA, $GPGSV.

VisualGPSView screenshot from VisualGPC LLC.
VisualGPSView screenshot from VisualGPC LLC.

The following screen, focused on the time capabilities of GPS, requires a slightly different set of NMEA messages : $GPGGA or $GPRMC or $GPZDA, $GPGSA, $GPGSV.

NMEATime screen shot by VisualGPC LLC
NMEATime screen shot by VisualGPC LLC.

The above screenshot examples are useful for the general GPS user.For high - precision GPS mapping and surveying, the $GPGST message is particularly useful.In fact, I would say it’s a requirement for high - precision users.The reason is that GPS metadata is very important for the high - precision user as a method of assisting in determining the quality of a particular GPS coordinate.Typical GPS real - time metadata used in understanding the quality of the GPS coordinate include : PDOP, # of satellites tracked, correction method, and horizontal / vertical standard deviation values.If a GPS receiver user has the ability to see this information in the field during data collection, they have a level of confidence in the precision of the GPS data they are collecting.If you’ve used RTK before, you probably recall the familiar HRMS(horizontal RMS) and VRMS(vertical RMS) values displayed on your data collection device.The $GPGST message generates those values.

DD.MMMMMMM, DDMM.MMMMM, or DDMMSS.SSSSS


*/

#include <string.h>
//#include "XPLDirectCommon.h"
#include "CWSerial.h"
#include "Config.h"

Config::Config(char *inFileName)
{

    strncpy(_cfgFileName, inFileName, 512);
    _cfgFileName[511] = 0;
    
    FILE* cfgFile;
    _validConfig = false;
    
   
    if (cfgFile = fopen(_cfgFileName, "r"))
        fclose(cfgFile);
    else
    {
        fprintf(errlog, "***Configuration file %s doesn't exist, please replace...\n", _cfgFileName);
        return;
       // createNewConfigFile();
    }
    
    fprintf(errlog, "Opening configuration file %s... ", _cfgFileName);
    
	//config_setting_t* root, * setting, * group, * array;
	config_init(&_cfg);

    if (!config_read_file(&_cfg, _cfgFileName))
    {
        fprintf(errlog, "Error: %s  Line:%i\r\n", config_error_text(&_cfg), config_error_line(&_cfg));
        config_destroy(&_cfg);
           
    }
    else                                      
        fprintf(errlog, "Success.\n");
  
    _validConfig = true;
 
}

Config::~Config()
{
    if (_validConfig) return;

    config_write_file(&_cfg, _cfgFileName);
   
	config_destroy(&_cfg);
}


// commit changes to the configuration file
void Config::saveFile(void)
{
    if (!config_write_file(&_cfg, _cfgFileName))
    {
        fprintf(errlog, "Config::saveFile Error: %s  Line:%i\r\n", config_error_text(&_cfg), config_error_line(&_cfg));
        return;
    }
    else
    {
       // fprintf(errlog, "Config::saveFile: Success updating configuration file.\n");
       

    }
}


// getSerialLogFlag
int Config::getSerialLogFlag(void)
{
    int flag = 0;
 
    if (!_validConfig) return 0;

    if (config_lookup_int(&_cfg, "CWSerialNMEAPlugin.logSerialData", &flag) == CONFIG_TRUE)
        fprintf(errlog, "Config module found CWSerialNMEAPlugin.logSerialData value %i\r", flag);
    else 
        fprintf(errlog, "*** Config module returned CONFIG_FALSE looking up value CESerialNMEAPlugin.logSerialData \r\n");

    return flag;
}

void Config::setSerialLogFlag(int flag)
{
    if (!_validConfig) return;

    config_setting_t* flagSetting = config_lookup(&_cfg, "CWSerialNMEAPlugin.logSerialData");

    if (flagSetting == NULL)
    {
        fprintf(errlog, "*** Config module unable to locate path CWSerialNMEAPlugin.logSerialData during write\r\n");
        return;
    }

    if (config_setting_set_int(flagSetting, flag) == CONFIG_TRUE)
        fprintf(errlog, "Config module set CWSerialNMEAPlugin.logSerialData to value %i\r", flag);
    else
        fprintf(errlog, "***Config module returned error setting CWSerialNMEAPlugin.logSerialData to value %i\r\n", flag);
}


// getRefreshRate
int Config::getRefreshRate(int *refreshRate)
{
       
    if (!_validConfig) return 0;

    if (config_lookup_int(&_cfg, "CWSerialNMEAPlugin.refreshRate", refreshRate) == CONFIG_TRUE)
        fprintf(errlog, "Config module found CWSerialNMEAPlugin. refreshRate value %i\r\n", *refreshRate);
    else
        fprintf(errlog, "*** Config module returned CONFIG_FALSE looking up value CWSerialNMEAPlugin.refreshRate \r\n");

    
}


// getComPort
int Config::getSerialParameters(int *comPort, int *baudRate)
{
    
    if (!_validConfig)
    {
        fprintf(errlog, "Invalid config file\r");
        return -1;
    }
   

    if (config_lookup_int(&_cfg, "CWSerialNMEAPlugin.comPort", comPort) == CONFIG_TRUE)
        fprintf(errlog, "Config module found CWSerialNMEAPlugin.comPort value %i\r", *comPort);
    else
        fprintf(errlog, "*** Config module returned CONFIG_FALSE looking up value CWSerialNMEAPlugin.comPort \r");

    if (config_lookup_int(&_cfg, "CWSerialNMEAPlugin.baudRate", baudRate) == CONFIG_TRUE)
        fprintf(errlog, "Config module found CWSerialNMEAPlugin.baudRate value %i\r\n", *baudRate);
    else
        fprintf(errlog, "*** Config module returned CONFIG_FALSE looking up value CWSerialNMEAPlugin.baudRate \r");

    return 0;

}


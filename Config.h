#pragma once
#ifndef CONFIGCLASS_H_INCLUDED
#define CONFIGCLASS_H_INCLUDED

#include "libconfig.h"



extern FILE* errlog;

class Config
{
private:
    
    char _cfgFileName[512];

    config_t _cfg;
       
   // void createNewConfigFile(void);
   


public:
   
    Config(char *inFileName);
    ~Config();
    void saveFile(void);

    int getSerialLogFlag(void);
    void setSerialLogFlag(int);
    int getRefreshRate(int *refreshRate);
    int getSerialParameters(int *comPort, int *baudRate);
    int getDatarefCount(void);
    int getDatarefElement(int element, const char** outName);
    // local globals
    int _validConfig;
};

#endif 


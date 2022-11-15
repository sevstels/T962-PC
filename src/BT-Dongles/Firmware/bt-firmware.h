//==============================================================================
//File name:   "bt-firmware.h"
//Purpose:      Header File
//Version:      1.00
//Copyright:    (c) 2022, Akimov Vladimir  E-mail: decoder@rambler.ru		
//==============================================================================
#ifndef _BT_FW_H_
#define _BT_FW_H_
 
#include <string>

#define	DONGLES_NUM 7

typedef struct 
{
  unsigned char numbers;
  std::string name[DONGLES_NUM];
  std::string manufacturer[DONGLES_NUM]; 
  unsigned short vid[DONGLES_NUM];
  unsigned short pid[DONGLES_NUM];
  unsigned short id[DONGLES_NUM];
  unsigned char *pFirmware[DONGLES_NUM];
  unsigned long firmwareLength[DONGLES_NUM];

} ÂT_manufacturers;

#ifdef  __cplusplus
extern "C" 
{
#endif //end __cplusplus

#ifdef __cplusplus
}
#endif //end __cplusplus

#endif
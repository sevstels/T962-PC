//==============================================================================
//File name:   "beacon_msg.h"
//Purpose:      Header File
//Version:      1.00
//Copyright:    (c) 2017, Akimov Vladimir  E-mail: decoder@rambler.ru		
//==============================================================================
#ifndef _BCN_MSG_H_
#define _BCN_MSG_H_

//-----------------------------------------------
//Structure 
//-----------------------------------------------
#pragma pack(push, 1)
typedef struct Beacon_tx_struct
{
  //---- 
  unsigned char  cmd_length;		 //1
  unsigned char  cmd_header;		 //1
  unsigned char  tx_counter;         //1
  //
  unsigned char  advert_interval_ms; //1
  unsigned long  pc_clock_us; 		 //4
  signed short   frame_offset_us;	 //2
  unsigned short frame_rate_us;		 //2
  unsigned short frame_left_right;	 //2  14
  unsigned short cmd_param[32-14];	 //min=4 
     
} Beacon_data;
#pragma pack(pop)

#endif

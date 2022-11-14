//==============================================================================
//File name:    "edits.cpp"
//Purpose:      Source File
//Version:      1.00
//Copyright:    (c) 2016, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "stdafx.h"
#include "Page2.h"
//#include "candll.h"
#include "cmd.h"

//---- Mem Leakage Debug
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
//----

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------
//Function:
//------------------------------------------------------------------------------    
int CPage2::CheckBoxs(MSG *pMsg)
{   /*
 //Check message from press ENTER
 //UINT msg = pMsg->message; 
 if((pMsg->message !=WM_LBUTTONDOWN) || (pMsg->wParam != MK_LBUTTON)) return 0;
 //   (msg != )) return 0;     WM_LBUTTONUP
 //else return 0; 
 //else if(msg != WM_LBUTTONUP) return 0;  
     /*) return 0; */
   /*
 int result;
 unsigned char cmd, param;

 //Get current settings
 int p = m_check_onoff_P.GetCheck();
 int i = m_check_onoff_I.GetCheck();
 int d = m_check_onoff_D.GetCheck();

 //---------------------------------------------------------
 if(pMsg->hwnd==m_check_onoff_P.m_hWnd)
   {if(p==1)p = 0; else p = 1; 
    /*Controls_Update(2); return 1;*//*}
 if(pMsg->hwnd==m_check_onoff_I.m_hWnd)
   {if(i==1)i = 0; else i = 1;  
   /*Beep(800, 20);Controls_Update(3); return 1;*//*}
 if(pMsg->hwnd==m_check_onoff_D.m_hWnd)
   {if(d==1)d = 0; else d = 1;  
   /*Beep(1000, 20);Controls_Update(4); return 1;*//*}  
   
 //---- disable PID
 if(p==0 && i==0 && d==0) 
 {
   Controls_PIDEnable(false);
   pDevice->PID_mode[PID_channel] = 1;
   m_radio_pid_disable.SetCheck(1);
   m_radio_pid_enable.SetCheck(0);
   m_radio_pid_remote_dac.SetCheck(0);
   return 1;
 } 
 
 //---------------------------------------------------------
 if(pMsg->hwnd==m_check_onoff_P.m_hWnd){cmd = 1; Controls_Update(2); return 1;}
 if(pMsg->hwnd==m_check_onoff_I.m_hWnd){cmd = 1; Controls_Update(3); return 1;}
 if(pMsg->hwnd==m_check_onoff_D.m_hWnd){cmd = 1; Controls_Update(4); return 1;}

  */
   /*
 //---- enable PID
 if(pid_on==1) 
 {
   if(msg == WM_LBUTTONUP) 
      Controls_PIDEnable(true);
 } */

 //if(pMsg->hwnd==m_check_DAC_hold.m_hWnd)cmd = 1;

   //{
   
    
    /* result = m_check_PulseMonitorng.GetCheck();
     cmd = CMD_PULSE_MONITORING;
     if(result) param = 1;
     else param = 0;
     //Send 2 Byte    
     ///CAN_SendCommand(pCAN, CAN_board_ID, CAN_CMD_CHANNEL, cmd, param, 0); */
     //Beep(1000, 5);  
     //return 1;
   //}
    
   return 0;
}

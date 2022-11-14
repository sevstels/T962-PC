//==============================================================================
//File name:    "controls2.cpp"
//Purpose:      Source File
//Version:      1.00
//Copyright:    (c) 2022, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "stdafx.h"
#include "Page2.h"
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
void CPage2::Controls_Ini(void)
{
  m_check_LogFix.SetCheck(FALSE);
  m_radio_tuning.SetCheck(FALSE);
  m_radio_normal.SetCheck(TRUE);

}

//------------------------------------------------------------------------------
//Function:
//------------------------------------------------------------------------------
void CPage2::Controls_Enable(BOOL on_off)
{
  //Edit controls
  m_KP.EnableWindow(on_off);
  m_KI.EnableWindow(on_off);
  m_KD.EnableWindow(on_off);
  m_REF.EnableWindow(on_off);

  m_PID_Period.EnableWindow(FALSE);
  m_FAN_Gain.EnableWindow(on_off);
  m_check_LogFix.EnableWindow(on_off);
  m_radio_tuning.EnableWindow(on_off);
  m_radio_normal.EnableWindow(on_off);

  //Buttons
  m_but_set_default.EnableWindow(on_off);
  m_but_PID_test.EnableWindow(on_off);   
  m_but_PID_Manual.EnableWindow(on_off);
  m_but_test_step.EnableWindow(on_off);
  m_but_test_linerity.EnableWindow(on_off);
  m_but_PID_AutoTune.EnableWindow(FALSE);
}

//------------------------------------------------------------------------------
//Function:
//------------------------------------------------------------------------------
void CPage2::Controls_PIDEnable(bool on_off)
{
  //Edit controls
  m_KP.EnableWindow(on_off);
  m_KI.EnableWindow(on_off);
  m_KD.EnableWindow(on_off);
  m_REF.EnableWindow(on_off);
     
  //Check Box`s
}

//------------------------------------------------------------------------------
//Function:
//------------------------------------------------------------------------------
void CPage2::Controls_PIDDisable(void)
{
  //Edit controls
  m_KP.EnableWindow(0);
  m_KI.EnableWindow(0);
  m_KD.EnableWindow(0);
  m_REF.EnableWindow(0);
}

//------------------------------------------------------------------------------
//Function:
//------------------------------------------------------------------------------
void CPage2::Controls_PIDWind(bool on_off)
{
  //Edit controls
  m_REF.EnableWindow(on_off);
}

//------------------------------------------------------------------------------
//Function:
//------------------------------------------------------------------------------
void CPage2::Controls_Update(char val_number)
{
  CString txt;
  float  buf;
  double dbuf, dstep=0, div=0;
  BOOL on_off;
  
  //---- REF
  txt.Format("%.2f", pDevice->PID.REF);
  m_REF.SetWindowTextA(txt);
  m_REF.UpdateWindow();
  	 
  //---- P
  buf = (float)pDevice->PID.P;
  txt.Format("%.2f", buf);
  m_KP.SetWindowTextA(txt);
  m_KP.UpdateWindow();
  //m_KP.EnableWindow(on_off);

  //---- KI
  buf = (float)pDevice->PID.I;
  txt.Format("%.4f", buf);
  m_KI.SetWindowTextA(txt);
  m_KI.UpdateWindow();
  //m_KI.EnableWindow(on_off);

  //---- D
  buf = (float)pDevice->PID.D;
  txt.Format("%.2f", buf);
  m_KD.SetWindowTextA(txt);
  m_KD.UpdateWindow();
  //m_KD.EnableWindow(on_off);

  //---- Period
  txt.Format("%d", pDevice->PID.period);
  m_PID_Period.SetWindowTextA(txt);
  m_PID_Period.EnableWindow(FALSE);
   
  txt.Format("%.1f", pDevice->fan_gain);
  m_FAN_Gain.SetWindowTextA(txt);

  
  //PID Log
  m_check_LogFix.SetCheck(TRUE);
  PID_log = m_check_LogFix.GetCheck();


/*
  //---- Low Noise Mode
  if(pDevice->Setup.LowNoiseMode==true) m_check_low_noise_mode.SetCheck(1);
  else{ m_check_low_noise_mode.SetCheck(1);}
  
  //---- PID /DAC Mode
  if(pDevice->PID.chn_enable&(1<<PID_channel)) 
   {
     m_radio_pid_enable.SetCheck(1); 
     m_radio_pid_remote_dac.SetCheck(0);
     m_check_onoff_P.EnableWindow(TRUE);
     m_check_onoff_I.EnableWindow(TRUE);
     m_check_onoff_D.EnableWindow(TRUE);
   }
  else
   {
     m_radio_pid_enable.SetCheck(0); 
     m_radio_pid_remote_dac.SetCheck(1);
     Controls_PIDEnable(false);
   }
  Controls_PIDEnable(TRUE); 
  //Get current settings
  int p = pDevice->PID.KP_en;
  int i = pDevice->PID.KI_en;
  int d = pDevice->PID.KD_en;
  //---- disable ALL PID
  if(p==0 && i==0 && d==0) 
  {
	pDevice->PID.chn_enable &= ~(1<<PID_channel);
    if(m_radio_pid_enable.GetCheck()) Controls_PIDWind(FALSE);
    else Controls_PIDWind(TRUE);
  }	 */
}

/*
//------------------------------------------------------------------------------
//Function:
//------------------------------------------------------------------------------
void CLPESDlg::BoardControls_Update(void)
{  
  CString txt, msg, id;
  msg.Empty();
  txt = "Control Board ID:\t\t0x"; 
  
  id.Format("%X",CAN_board_ID);
  txt += id;
  txt += "\r\n";
  msg += txt;
  
  //--
  txt = "Control output:\t\t";
  if(Board.OutputMode == 0){txt += "Disabled"; m_check_SensorOutput.SetCheck(0);}
  else {txt += "Enabled"; m_check_SensorOutput.SetCheck(1);}  
  txt +="\r\n";
  msg += txt;
  
  //--
  txt = "Device mode:\t\t";
  if(Board.Mode == 0){txt += "Ready for use";}
  else {txt += "Focus calibr";}  
  txt +="\r\n";
  msg += txt;
  
  //--
  msg += "Attached Sensors:\r\n";
  for(int i=0; i<8; i++)
   {
      char ctxt[20];
      memset(ctxt,0,sizeof(ctxt));
      memcpy(ctxt, &(Board.AttachedName[i][0]), sizeof(ctxt));
      CString strtxt(ctxt);
      
      if(strtxt!="")
      {     
         txt.Format("In %d: %s", i, ctxt, Board.AttachedID[i]);
         msg += txt;
         if(Board.AttachedID[i]!=0){txt.Format(",\tID: 0x%X\r\n", Board.AttachedID[i]);}
         else txt = "\r\n";
         msg += txt;
      } 
   } 

  //--
  msg += "Test pulse Duration:\t\t";
  txt.Format("%d", Board.TestPulseDuration);                                     
  m_edit_duration.SetWindowTextA(txt);
  msg += txt + "uS\r\n";
  
  //--
  msg += "Test pulse Period:\t\t";
  txt.Format("%d", Board.TestPulsePeriod);                                     
  m_edit_period.SetWindowTextA(txt);
  msg += txt + "uS\r\n";
  
  msg += "Pulse Length monitor:\t";
  if(Board.PulseMonitoring==0){txt = "Disabled"; m_check_PulseMonitorng.SetCheck(0);}
  else {txt = "Enabled"; m_check_PulseMonitorng.SetCheck(1);}  
  msg += txt + "\r\n";
  
  //--
  txt.Format("Power supply:\t\t%2.2fv,", Board.Power);
  if(Board.Power < 10) txt += "Err!";
  else txt += " OK";
  txt +="\r\n";
  msg += txt;  
  
  //--
  msg += "Device Firmware:\t";
  txt.Format("ver%d.",(Board.Firmware_version>>4));                                     
  msg += txt;
  txt.Format("%d",(Board.Firmware_version)&0x0F);
  msg += txt;
  txt.Format("build:%d",Board.Firmware_build);
  msg += txt + "\r\n";
  
  m_edit_monitor.SetWindowTextA(msg);
  m_edit_monitor.UpdateWindow();

  if(Sensor.Firmware_version < SN_FIRMWARE_VERSION_LOWER)
   {
        AfxMessageBox(_T("Attention!\nThe Control Board operation under outdated \
      (old) firmware version!\nDevice required Firmware Update!"));
   }
} 
 
//------------------------------------------------------------------------------
//Function:
//------------------------------------------------------------------------------
void CLPESDlg::SensorControls_Update(void)
{ 
  CString txt, msg, id;
  msg.Empty();
  txt = "Sensor ID:\t0x"; 
  
  id.Format("%X",CAN_sensor_ID);
  txt += id;
  txt += "\r\n";
  msg += txt;
  
  //--
  txt.Format("Integrator pos:\t%d\r\n", Sensor.TAU_value+1);
  msg += txt;
  m_slider_TAU.SetPos(Sensor.TAU_value);

  //--
  txt.Format("Reference pos:\t%d\r\n", Sensor.REF_value);
  msg += txt;
    m_slider_REF.SetPos(Sensor.REF_value);
  
  //--
  txt = "Control output:\t";
  if(Sensor.Output == 0){txt += "Disabled"; m_check_SensorOutput.SetCheck(0);}
  else {txt += "Enabled"; m_check_SensorOutput.SetCheck(1);}  
  txt +="\r\n";
  msg += txt;
  
  //--
  txt = "Device mode:\t";
  if(Sensor.Mode == 0){txt += "Ready for use";}
  else {txt += "Focus calibr";}  
  txt +="\r\n";
  msg += txt;
  
  //--
  txt = "Light Scale:\t";
  if((Sensor.Sound_Scale&0x01) == 0){txt += "Linear";}
  else {txt += "LOG, dB:0-100";}  
  txt +="\r\n";
  msg += txt;
  
  //--
  txt = "Test Sound:\t";
  if((Sensor.Sound_Scale&0x10) == 0x10){txt += "Enabled";}
  else {txt += "Disabled";}  
  txt +="\r\n";
  msg += txt;
              
  //--
  txt.Format("DC/DC Power:\t%2.2fv,", Sensor.Power);
  if(Sensor.Power < 7.4) txt += "Err!";
  else txt += " OK";
  txt +="\r\n";
  msg += txt;

  //--
  char sound, scale;
  sound = (Sensor.Sound_Scale>>4);
  scale = (Sensor.Sound_Scale&0x01);

  if(sound==1) m_check_Sound.SetCheck(1);
  else m_check_Sound.SetCheck(0);
  
  if(scale==1) m_check_Scale.SetCheck(1);
  else m_check_Scale.SetCheck(0);

  //--
  msg += "Photo Diode:\t";
  txt.Format("%s\r\n", Sensor.PhotoDiode);                                     
  msg += txt;

  //--
  msg += "Device Firmware:\t";
  txt.Format("ver %d.",(Sensor.Firmware_version>>4));                                     
  msg += txt;
  txt.Format("%d,",(Sensor.Firmware_version)&0x0F);
  msg += txt;
  txt.Format(" build %d",Sensor.Firmware_build);
  msg += txt + "\r\n";
  
  m_edit_monitor.SetWindowTextA(msg);
  m_edit_monitor.UpdateWindow();

  if(Sensor.Firmware_version < SN_FIRMWARE_VERSION_LOWER)
   {
        AfxMessageBox(_T("Attention!\nThe Sensor Device operation under outdated \
      (old) firmware version!\nDevice required Firmware Update!"));
   }
 
 // m_progress.ShowWindow(false);
 // m_but_Default.EnableWindow(true); 
}                                     */

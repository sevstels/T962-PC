//==============================================================================
//File name:   "dlg.h"
//Purpose:      Header File
//Version:      1.00
//Copyright:    (c) 2022, Akimov Vladimir  E-mail: decoder@rambler.ru			 ..\..\..\..\..\LibCPP\ProtocolEx
//==============================================================================
#include "stdafx.h"
#include "afxwin.h"
#include "tooltip.h"
#include "tec-resource.h"
#include "AppWinReg.h"
#include "dialog-pos.h"
#include "cstatic-color.h"
#include "app-setup.h"

#ifndef _DLG_BT_SETUP_CLASS_H_
#define _DLG_BT_SETUP_CLASS_H_

//a custom Windows message
#define UWM_DIALOG_MESSAGES (WM_APP + 2) 

//----
#define DLG_CLOSE  0
#define DLG_SENSOR 2
#define DLG_WRITE_DEVSETUP 2

#define SET_GAIN   0x01
#define SET_SMUX   0x02
#define SET_ATIME  0x03
#define SET_ASTEP  0x04
#define SET_CALIBR 0x05
#define SET_LED	   0x06	  

//#pragma once                
//------------------------------------------------------------------------------
//Embedded dialog class 
//------------------------------------------------------------------------------
class CBTDlg : public CDialog
{
  DECLARE_DYNAMIC(CBTDlg)

  //Construction
  public:
  
  //---- Construction
  //standard constructor
  CBTDlg(); 
 ~CBTDlg();
  
  //Resource ID
  enum {IDD = IDD_DIALOG1};

  //Implementation
  CAppWinReg *pREG;
  CToolTip ToolTip;
//  AMS_Setup *pDev;
  CDPOS POS;
  HWND hWndParent;

  int dev_connected;
  int show_cnh_name;
  int channel;
  CString channel_name;

  //----
  CButton m_but_default;
  CButton m_but_mux_off;
  CButton m_but_mux_default;
  CComboBox m_combo_gain;
  CComboBox m_combo_calibration;
  CEdit m_edit_intg_step;
  CEdit m_edit_intg_time;
  CStatic m_group_intgr;
 // CEdit m_edit_led_current;
 // CButton m_check_led_on;

  //----
  CStaticColor m_F1_2;
  CStaticColor m_F1_32;
  //----
  CStaticColor m_F2_10;
  CStaticColor m_F2_25;
  //----
  CStaticColor m_F3_1;
  CStaticColor m_F3_31;
  //----
  CStaticColor m_F4_11;
  CStaticColor m_F4_26;
  //----
  CStaticColor m_F5_13;
  CStaticColor m_F5_19;
  //----
  CStaticColor m_F6_8;
  CStaticColor m_F6_29;
  //----
  CStaticColor m_F7_14;
  CStaticColor m_F7_20;
  //----
  CStaticColor m_F8_7;
  CStaticColor m_F8_28;
  //----
  CStaticColor m_FL_39;
  CStaticColor m_CL_17;
  CStaticColor m_CL_35;
  CStaticColor m_IR_38;
  //----
  CStaticColor m_static_f[11];

  //----func
  void CloseDlg(void);
  void SetTooltip(void);
  void Controls_Enable(BOOL on_off);
  void Controls_Update(void);
  int  ParseValue(CEdit *pEdit, short min, int max);
  //----
  void SMUX_ControlIni(void);
  void SMUX_Selector(int f, int f_num);
  void SMUX_PopupMenu(CWnd* pWnd, CRect *pRect);
  void SMUX_Load(char setup);
  void SMUX_Show(char setup, char position);
  void SMUX_Map(char position, char &offset, char &index);
  char SMUX_GetVal(char position);
  void SMUX_SetVal(char address, char shift, char value);
  //----
  long CalcIntegrationTime(unsigned short astep, unsigned char atime);


  protected:
  DECLARE_MESSAGE_MAP()
  unsigned char enable;
  unsigned char diode_position;
  unsigned char diode_channel;

  public:
  //DDX/DDV support 
  virtual BOOL OnInitDialog(void);
  virtual BOOL PreTranslateMessage(MSG* pMsg);
  virtual void DoDataExchange(CDataExchange* pDX);  
  virtual void OnOK();  
  virtual void OnCancel();
  afx_msg void OnBnClickedOk();
  afx_msg void OnButtonSetDefault();
  afx_msg void OnComboGain();
  afx_msg void OnComboCalibration();
  //afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
  afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
  
  //----
  afx_msg void OnF1_2();
  afx_msg void OnF1_32();
  afx_msg void OnF2_10();
  afx_msg void OnF2_25();
  afx_msg void OnF3_1();
  afx_msg void OnF3_31();
  afx_msg void OnF4_11();
  afx_msg void OnF4_26();
  afx_msg void OnF5_13();
  afx_msg void OnF5_19();
  afx_msg void OnF6_8();
  afx_msg void OnF6_29();
  afx_msg void OnF7_14();
  afx_msg void OnF7_20();
  afx_msg void OnF8_7();
  afx_msg void OnF8_28();
  afx_msg void OnFL_39();
  afx_msg void OnCL_17();
  afx_msg void OnCL_35();
  afx_msg void OnIR_38();
  //----
  afx_msg void OnConnectCh1();
  afx_msg void OnConnectCh2();
  afx_msg void OnConnectCh3();
  afx_msg void OnConnectCh4();
  afx_msg void OnConnectCh5();
  afx_msg void OnConnectCh6();
  afx_msg void OnDisconnect();
  //----
  afx_msg void OnButtonSMUX_OffAll();
  afx_msg void OnButtonSMUX_Default();
  
  afx_msg void OnCheckLedOn();
  afx_msg void OnButtonOpenDoc();
};

#endif

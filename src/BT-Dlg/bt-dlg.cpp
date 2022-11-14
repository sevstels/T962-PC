//==============================================================================
//File name:    "dlg_sensor.cpp"
//Purpose:      Source File
//Version:      1.00
//Copyright:    (c) 2022, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "stdafx.h"
#include "bt-dlg.h"

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

//---- Macro
IMPLEMENT_DYNAMIC(CBTDlg, CDialog)

//------------------------------------------------------------------------------
//Constructor
//------------------------------------------------------------------------------
CBTDlg::CBTDlg()
{ 
//  pDev = NULL;
}

//------------------------------------------------------------------------------ 
//Destructor
//------------------------------------------------------------------------------
CBTDlg::~CBTDlg()
{
}

//------------------------------------------------------------------------------ 
//
//------------------------------------------------------------------------------
void CBTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBTDlg)

	//}}AFX_DATA_MAP

//	DDX_Control(pDX, IDC_CHECK_LED_ON, m_check_led_on);
}														  

//------------------------------------------------------------------------------ 
//mesages
//------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CBTDlg, CDialog)
  //{{AFX_MSG_MAP(CBTDlg)
  ON_WM_PAINT() 
  ON_WM_DESTROY()
  ON_WM_SYSCOMMAND()
  //ON_WM_CONTEXTMENU()
  ON_WM_QUERYDRAGICON()
  ON_WM_WINDOWPOSCHANGED()


END_MESSAGE_MAP()

//------------------------------------------------------------------------------ 
//Dialog initialization
//------------------------------------------------------------------------------
BOOL CBTDlg::OnInitDialog()
{
  CDialog::OnInitDialog();
  /*
  CString txt;
  //номер канала который обслуживает сейчас диалог
  txt.Format("Sensor %s: %d", channel_name, channel+1);
  this->SetWindowTextA(txt);
  POS.SetWindowPositon(this->m_hWnd);

  //----
  m_F1_2.SetBackgroundColor(RGB(140, 67,191)); //F1
  m_F1_32.SetBackgroundColor(RGB(140, 67,191));//F1
  m_F1_2.SetTextColor(RGB(255,255,255));       //F1
  m_F1_32.SetTextColor(RGB(255,255,255));      //F1
  m_static_f[0].SetBackgroundColor(RGB(140,67,191));//F1
  m_static_f[0].SetTextColor(RGB(255,255,255));     //F1

  //----
  m_F2_10.SetBackgroundColor(RGB(0,20,208)); //F2
  m_F2_25.SetBackgroundColor(RGB(0,20,208)); //F2
  m_F2_10.SetTextColor(RGB(255,255,255));    //F2
  m_F2_25.SetTextColor(RGB(255,255,255));	 //F2
  m_static_f[1].SetBackgroundColor(RGB(0,20,208));//F2
  m_static_f[1].SetTextColor(RGB(255,255,255));   //F2
  //----
  m_F3_1.SetBackgroundColor(RGB(0,176,236)); //F3
  m_F3_31.SetBackgroundColor(RGB(0,176,236));//F3
  m_static_f[2].SetBackgroundColor(RGB(0,176,236));//F3  
  //----
  m_F4_11.SetBackgroundColor(RGB(0,254,205));//F4
  m_F4_26.SetBackgroundColor(RGB(0,254,205));//F4
  m_static_f[3].SetBackgroundColor(RGB(0,254,205));//F4
  //----
  m_F5_13.SetBackgroundColor(RGB(0,174,89)); //F5
  m_F5_19.SetBackgroundColor(RGB(0,174,89)); //F5
  m_static_f[4].SetBackgroundColor(RGB(0,174,89));//F5
  //----
  m_F6_8.SetBackgroundColor(RGB(255,254,73));//F6
  m_F6_29.SetBackgroundColor(RGB(255,254,73));//F6
  m_static_f[5].SetBackgroundColor(RGB(255,254,73));//F6
  //---- 
  m_F7_14.SetBackgroundColor(RGB(255,189,57));//F7
  m_F7_20.SetBackgroundColor(RGB(255,189,57));//F7
  m_static_f[6].SetBackgroundColor(RGB(255,189,57));//F7
  //----
  m_F8_7.SetBackgroundColor(RGB(202,0,22)); //F8
  m_F8_28.SetBackgroundColor(RGB(202,0,22));//F8
  m_F8_7.SetTextColor(RGB(255,255,255)); //F8
  m_F8_28.SetTextColor(RGB(255,255,255));//F8
  m_static_f[7].SetBackgroundColor(RGB(202,0,22));//F8
  m_static_f[7].SetTextColor(RGB(255,255,255));
  //----
  m_CL_17.SetBackgroundColor(RGB(255,255,255));//CLEAR  
  m_CL_35.SetBackgroundColor(RGB(255,255,255));//CLEAR  
  m_static_f[9].SetBackgroundColor(RGB(255,255,255));//CL
  //----
  m_IR_38.SetBackgroundColor(RGB(121,0,12));//NIR
  m_IR_38.SetTextColor(RGB(255,255,255)); 
  m_static_f[8].SetBackgroundColor(RGB(121,0,12));//NIR
  m_static_f[8].SetTextColor(RGB(255,255,255));
  //----
  m_FL_39.SetBackgroundColor(RGB(250,250,250));//FLCKER
  m_static_f[10].SetBackgroundColor(RGB(250,250,250));//FL

  //m_button_1. m_bTransparent = false;//reg for use bg color
  //m_button_1.set SetFaceColor(RGB(211, 84, 0), true);

  //Ini all
  m_combo_gain.ResetContent();
  m_combo_gain.InsertString(0, "x 0.5");
  m_combo_gain.InsertString(1, "x 1");
  m_combo_gain.InsertString(2, "x 2");
  m_combo_gain.InsertString(3, "x 4");
  m_combo_gain.InsertString(4, "x 8");
  m_combo_gain.InsertString(5, "x 16");
  m_combo_gain.InsertString(6, "x 32");
  m_combo_gain.InsertString(7, "x 64");
  m_combo_gain.InsertString(8, "x 128");
  m_combo_gain.InsertString(9, "x 256");
  m_combo_gain.InsertString(10,"x 512");
  m_combo_gain.SetCurSel(pDev->gain); 
  
  //----
  m_combo_calibration.ResetContent();
  m_combo_calibration.InsertString(0, "Auto Zero Calibration Off");		 
  m_combo_calibration.InsertString(1, "Every integration cycle");
  m_combo_calibration.InsertString(2, "Every 2 cycles");
  m_combo_calibration.InsertString(3, "Every 3 cycles");
  m_combo_calibration.InsertString(4, "Every 4 cycles");
  m_combo_calibration.InsertString(5, "Every 5 cycles");
  m_combo_calibration.InsertString(6, "Every 6 cycles");
  m_combo_calibration.InsertString(7, "Every 7 cycles");
  m_combo_calibration.InsertString(8, "Every 8 cycles");
  m_combo_calibration.InsertString(9, "Every 9 cycles"); 
  m_combo_calibration.InsertString(10,"Every 10 cycles"); 
  //m_combo_calibration.InsertString(11,"Only Before first measure"); 
  m_combo_calibration.SetCurSel(pDev->calibration); 
 
  //---- 
  txt.Format("%d", pDev->reg_astep);
  m_edit_intg_step.SetWindowTextA(txt);
  m_edit_intg_step.UpdateWindow();

  //---- 
  txt.Format("%d", pDev->reg_atime);
  m_edit_intg_time.SetWindowTextA(txt);
  m_edit_intg_time.UpdateWindow();

  //----
  //long time = 0;
  //time = CalcIntegrationTime(pDev->reg_astep, pDev->reg_atime);
  txt.Format("Integration Time = %dmS ", pDev->integration_time_ms);
  m_group_intgr.SetWindowTextA(txt);

  /*  
  //---- LED 
  txt.Format("%d", pDev->led_current);
  m_edit_led_current.SetWindowTextA(txt);
  m_edit_led_current.UpdateWindow();
  m_check_led_on.SetCheck(pDev->led_enable);  */
  /*
  //Show F settings
  for(char i=0; i<40; i++)
  {
	//----
	char setup = SMUX_GetVal(i);
	SMUX_Show(setup, i);
  }	

  //----
  Controls_Enable((BOOL)dev_connected);
  //Controls_Enable(FALSE);

  //add tooltips
  struct prj_tooltip tooltips[] = 
  { 
    {&m_combo_gain, "Photodiode signal preamplifier"},
    {&m_edit_intg_step, "Integration Time step = 2.78uS x value"},
    {&m_edit_intg_time, "Time step x value"},
   // {&m_edit_led_current, "Light intensity"},
	{&m_but_mux_off, "Disconnect all photodiodes"},
    {&m_but_mux_default, "Group the same channels"},
/*  {&m_but_one_save, "Writes one sample to a file when pressed"},
    {&m_but_autosave, "Writes samples to a file every 10 ms"},
    {&m_but_help, "Show sensor manual pdf"},*/
 /*   //----
    {&m_F1_2,  "F1: 410nm"},
    {&m_F1_32, "F1: 410nm"},
    {&m_F2_10, "F2: 440nm"},
    {&m_F2_25, "F2: 440nm"},
    {&m_F3_1,  "F3: 470nm"},
    {&m_F3_31, "F3: 470nm"},
    {&m_F4_11, "F4: 510nm"},
    {&m_F4_26, "F4: 510nm"},
    {&m_F5_13, "F5: 550nm"},
    {&m_F5_19, "F5: 550nm"},
    {&m_F6_8,  "F6: 583nm"},
    {&m_F6_29, "F6: 583nm"},
    {&m_F7_14, "F7: 620nm"},
    {&m_F7_20, "F7: 620nm"},
    {&m_F8_7,  "F8: 670nm"},
    {&m_F8_28, "F8: 670nm"},
    {&m_FL_39, "FL: Flicker"},
    {&m_CL_17, "CL: Clear White"},
    {&m_CL_35, "CL: Clear White"},
    {&m_IR_38, "NIR: Near Infra Red"}
  };

  ToolTip.SetToolTip(FromHandle(this->m_hWnd), tooltips, _countof(tooltips)); 
*/  
  return TRUE;
}

//------------------------------------------------------------------------------
//Setting the integration time  𝑡𝑖𝑛𝑡 = (𝐴𝑇𝐼𝑀𝐸 + 1) × (ASTEP + 1) x 2.78uS
//------------------------------------------------------------------------------
long CBTDlg::CalcIntegrationTime(unsigned short astep, 
	                                      unsigned char atime)
{ 
  //ASTEP: 0-65534
  //ATIME: 0-255
  double integration_time = (double)2.78;
  integration_time *= (atime+1);
  integration_time *= (astep+1);

  long time_us = (long) integration_time;
  return time_us;
}

//------------------------------------------------------------------------------
//Function:
//------------------------------------------------------------------------------
void CBTDlg::Controls_Enable(BOOL on_off)
{  
  //Combo`s
  m_combo_gain.EnableWindow(on_off); 
  m_combo_calibration.EnableWindow(on_off);

  //Edit controls
  m_edit_intg_time.EnableWindow(on_off);
  m_edit_intg_step.EnableWindow(on_off);
//  m_edit_led_current.EnableWindow(on_off);

  //Buttons
  m_but_default.EnableWindow(on_off);
  //m_check_led_on.EnableWindow(on_off);
}

//------------------------------------------------------------------------------
//Function:
//------------------------------------------------------------------------------
void CBTDlg::Controls_Update(void)
{  
  CString txt;
 /*  
  //Combo`s
  m_combo_gain.SetCurSel(pDev->gain);
  m_combo_calibration.SetCurSel(pDev->calibration); 

  //---- 
  txt.Format("%d", pDev->reg_astep);
  m_edit_intg_step.SetWindowTextA(txt);
  m_edit_intg_step.UpdateWindow();

  //---- 
  txt.Format("%d", pDev->reg_atime);
  m_edit_intg_time.SetWindowTextA(txt);
  m_edit_intg_time.UpdateWindow();

  //----
  long time = 0;
  time = CalcIntegrationTime(pDev->reg_astep, pDev->reg_atime);
  txt.Format("Integration Time: %.2fmS", ((float)time)/1000);
  m_group_intgr.SetWindowTextA(txt);
*/

/*  //---- LED 
  txt.Format("%d", pDev->led_current);
  m_edit_led_current.SetWindowTextA(txt);
  m_edit_led_current.UpdateWindow();
  m_check_led_on.SetCheck(pDev->led_enable);
	 
  //Show F settings
  for(char i=0; i<40; i++)
  {
	//----
	char setup = SMUX_GetVal(i);
	SMUX_Show(setup, i);
  }	   */
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void CBTDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
  CDialog::OnWindowPosChanged(lpwndpos);
  POS.WindowPosChanged(this->m_hWnd, lpwndpos);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int CBTDlg::ParseValue(CEdit *pEdit, short min, int max)
{
  CString txt;
  int value;
  int result=0;

  int dlgid = pEdit->GetDlgCtrlID();
  result = GetDlgItemTextA(dlgid, txt);  
  
  //read or not, empty 
  if(result<1)
  {
    txt = "Can`t read it, enter a number";  
    ::AfxMessageBox(txt);
    return -1;
  }

  //read value in any format
  result = sscanf_s(txt, "%d",(int*)&value);
  
  if(result<1)
  {
    txt = "This is not digit!\r\nEnter a number";  
    ::AfxMessageBox(txt);
    return -1;
  }

  if(value<min || value>max)
   {
      txt.Format("Value must be\r\nbetween: %d... %d", min, max);      
      ::AfxMessageBox(txt);
      return -1;
   }
 
  return (int) value;
}

//------------------------------------------------------------------------------ 
//Press OK
//------------------------------------------------------------------------------
BOOL CBTDlg::PreTranslateMessage(MSG* pMsg)
{
  //Check message from press ENTER 
  if((WM_KEYDOWN == pMsg->message)&&(VK_RETURN == pMsg->wParam)) 
  {
   unsigned char cmd, value;

   //----    
   if(pMsg->hwnd==m_edit_intg_time.m_hWnd)
   { 
     int integration = ParseValue(&m_edit_intg_time, 0, 255);
     if(integration<0) return TRUE; 
///     pDev->reg_atime = (unsigned char) integration;
     
	 //----
	 long time = 0;
	 CString txt;
///     time = CalcIntegrationTime(pDev->reg_astep, pDev->reg_atime);
     txt.Format("Integration Time = %dmS", time/1000);
     m_group_intgr.SetWindowTextA(txt);
	 
	 //Send cmd to main dlg
     ::PostMessageA(hWndParent, UWM_DIALOG_MESSAGES, SET_ATIME, channel);
     return TRUE;
   }

   //----    
   if(pMsg->hwnd==m_edit_intg_step.m_hWnd)
   { 
     int integration = ParseValue(&m_edit_intg_step, 0, 65534);
     if(integration<0) return TRUE; 
//     pDev->reg_astep = (unsigned short) integration;
	 
	 //----  
	 long time = 0;
	 CString txt;
///     time = CalcIntegrationTime(pDev->reg_astep, pDev->reg_atime);
     txt.Format("Integration Time = %dmS", time/1000);
     m_group_intgr.SetWindowTextA(txt);

     //Send cmd to main dlg
     ::PostMessageA(hWndParent, UWM_DIALOG_MESSAGES, SET_ASTEP, channel);
     return TRUE;
   }
  }
  
  ToolTip.PreTranslateMessage(pMsg);      
  return CDialog::PreTranslateMessage(pMsg);
}  

//------------------------------------------------------------------------------ 
//Press Exit
//------------------------------------------------------------------------------
void CBTDlg::OnBnClickedOk()
{
  OnCancel();
  //TODO: Add your control notification handler code here
  //CDialog::OnOK();
}

//------------------------------------------------------------------------------ 
//
//------------------------------------------------------------------------------
void CBTDlg::OnOK()
{
  //CDialog::OnOK();
  int row, i;
  unsigned char mux = 0;

  for(row=0; row<10; row++)
  {
    for(i=0; i<7; i++)
    {
////      int check = m_radio_MUX[row][i].GetCheck();
      int check = 0;
      if(check)
      {
        i++;
        mux = (unsigned char)i;
        if(i==7) mux = 0; 
      }
    }
    TRACE2("Channel: F%d NUX%d\n", row+1, mux);
  }
}

//------------------------------------------------------------------------------ 
//
//------------------------------------------------------------------------------
void CBTDlg::OnCancel()
{
  CloseDlg(); 
}                 

//------------------------------------------------------------------------------ 
//
//------------------------------------------------------------------------------
void CBTDlg::CloseDlg(void)
{
  ::PostMessageA(hWndParent, UWM_DIALOG_MESSAGES, DLG_CLOSE, channel);
  CDialog::OnCancel();
}

//------------------------------------------------------------------------------ 
//
//------------------------------------------------------------------------------
void CBTDlg::OnButtonSetDefault()
{

}

//------------------------------------------------------------------------------ 
//
//------------------------------------------------------------------------------
void CBTDlg::OnComboGain()
{
  int select = m_combo_gain.GetCurSel();
///  pDev->gain = (unsigned char)select;
  //Send cmd to main dlg
  ::PostMessageA(hWndParent, UWM_DIALOG_MESSAGES, SET_GAIN, channel);
}

//------------------------------------------------------------------------------ 
//
//------------------------------------------------------------------------------
void CBTDlg::OnComboCalibration()
{
  int select = m_combo_calibration.GetCurSel();
 /// pDev->calibration = (unsigned char)select;
  //Send cmd to main dlg
  ::PostMessageA(hWndParent, UWM_DIALOG_MESSAGES, SET_CALIBR, channel);
}

//------------------------------------------------------------------------------ 
//
//------------------------------------------------------------------------------
void CBTDlg::OnF1_2() {SMUX_Selector(1,2);}
void CBTDlg::OnF1_32(){SMUX_Selector(1,32);}
void CBTDlg::OnF2_10(){SMUX_Selector(2,10);}
void CBTDlg::OnF2_25(){SMUX_Selector(2,25);}
void CBTDlg::OnF3_1() {SMUX_Selector(3,1);}
void CBTDlg::OnF3_31(){SMUX_Selector(3,31);}
void CBTDlg::OnF4_11(){SMUX_Selector(4,11);}
void CBTDlg::OnF4_26(){SMUX_Selector(4,26);}
void CBTDlg::OnF5_13(){SMUX_Selector(5,13);}
void CBTDlg::OnF5_19(){SMUX_Selector(5,19);}
void CBTDlg::OnF6_8() {SMUX_Selector(6,8);}
void CBTDlg::OnF6_29(){SMUX_Selector(6,29);}
void CBTDlg::OnF7_14(){SMUX_Selector(7,14);}
void CBTDlg::OnF7_20(){SMUX_Selector(7,20);}
void CBTDlg::OnF8_7() {SMUX_Selector(8,7);}
void CBTDlg::OnF8_28(){SMUX_Selector(8,28);}
void CBTDlg::OnFL_39(){SMUX_Selector(9,39);}
void CBTDlg::OnCL_17(){SMUX_Selector(10,17);}
void CBTDlg::OnCL_35(){SMUX_Selector(10,35);}
void CBTDlg::OnIR_38(){SMUX_Selector(11,38);}

//------------------------------------------------------------------------------ 
//
//------------------------------------------------------------------------------
void CBTDlg::SMUX_Selector(int chn, int position)
{
  int check = 0;

  TRACE2("Channel: F%d Position %d\n", chn, position);
    
  diode_channel = chn;
  diode_position = position;

  CRect Rect;
  CStatic *pElemet;

  switch(position)
  {
    case 2:  pElemet = &m_F1_2;  break;
    case 32: pElemet = &m_F1_32; break;
    case 10: pElemet = &m_F2_10; break;
    case 25: pElemet = &m_F2_25; break;
    case 1:  pElemet = &m_F3_1;  break;
    case 31: pElemet = &m_F3_31; break;
    case 11: pElemet = &m_F4_11; break;
    case 26: pElemet = &m_F4_26; break;
    case 13: pElemet = &m_F5_13; break;
    case 19: pElemet = &m_F5_19; break;
    case 8:  pElemet = &m_F6_8;  break;
    case 29: pElemet = &m_F6_29; break;
    case 14: pElemet = &m_F7_14; break;
    case 20: pElemet = &m_F7_20; break;
    case 7:  pElemet = &m_F8_7;  break;
    case 28: pElemet = &m_F8_28; break;
    case 39: pElemet = &m_FL_39; break;
    case 17: pElemet = &m_CL_17; break;
    case 35: pElemet = &m_CL_35; break;
    case 38: pElemet = &m_IR_38; break;

    default: return;
  }

  //индикация бордюра элемента
  DWORD style = pElemet->GetStyle();
  //https://docs.microsoft.com/ru-ru/cpp/mfc/reference/styles-used-by-mfc?view=msvc-160
  //pElemet->ModifyStyle(SS_CENTERIMAGE, SS_SUNKEN|SS_ETCHEDVERT, SWP_NOSIZE);
  //pElemet->UpdateWindow();

  //находим координаты элемента 
  pElemet->GetWindowRect(Rect);
  int id = m_F1_2.GetDlgCtrlID();
  CWnd* pWnd = GetDlgItem(id);
  
  //----
  SMUX_PopupMenu(pWnd, &Rect);

  //мультиплексор может быть сконфигурирован 3 битами 
  //0 = пиксель отключен / подключен к GND; 
  //от 1 до 6 = АЦП от 0 до АЦП 5)

  //Не упомянутые и серые пиксельные идентификаторы не 
  //используются и должны быть запрограммированы на “0”.
  
  //Для чтения и записи конфигурации пикселей используются 
  //первые 20 байт оперативной памяти, начинающиеся с адреса 00h.
  //Для удобства использования конфигурация пикселей хранится в виде кусочков 
  //внутри оперативной памяти (4 бита на конфигурацию пикселей, MSB не используется). 
  //Рекомендуется записать сразу 20 байт и настроить все пиксели 
  //вместе в рамках одной команды записи страницы.
}

//------------------------------------------------------------------------------ 
//
//------------------------------------------------------------------------------
void CBTDlg::OnConnectCh1(){SMUX_Load(1);}
void CBTDlg::OnConnectCh2(){SMUX_Load(2);}
void CBTDlg::OnConnectCh3(){SMUX_Load(3);}
void CBTDlg::OnConnectCh4(){SMUX_Load(4);}
void CBTDlg::OnConnectCh5(){SMUX_Load(5);}
void CBTDlg::OnConnectCh6(){SMUX_Load(6);}
void CBTDlg::OnDisconnect(){SMUX_Load(0);}

//------------------------------------------------------------------------------ 
//
//------------------------------------------------------------------------------  
void CBTDlg::SMUX_Load(char setup)
{
  //это ранее было установлено в SMUX_Selector
  //diode_channel = chn;
  //diode_position = position;

  char shift, address;
  setup &= 0x07; //3 bits only

  //определяем расположение байта конфига
  SMUX_Map(diode_position, shift, address);

  TRACE2("Byte: SMUX[%d], DataShift: <<%d, ", address, shift);
  if(setup!=0){TRACE1("Add to Channel: %d\n", setup);}
  else{TRACE("Channel: OFF\n");}

  //=======================================
  //забираем во временный буфер 
  unsigned char data;/// = pDev->smux[address];
  //очистка позиции
  data &= ~(0xf<<shift);
  //вставляем данные 
  data |= (setup<<shift);
  //сохраняем обратно
///  pDev->smux[address] = data; 

  //=======================================
  //отображение на экране 
  SMUX_Show(setup, diode_position);

  //Send cmd to main dlg
  ::PostMessageA(hWndParent, UWM_DIALOG_MESSAGES, SET_SMUX, channel);
}

//------------------------------------------------------------------------------
//контекстное меню
//------------------------------------------------------------------------------  
void CBTDlg::SMUX_Map(char pixel_id, char &offset, char &addr)
{
  //position 
  switch(pixel_id)
  {  
    case 1:  offset = 4; addr = 0;  break; //   
    case 2:  offset = 0; addr = 1;  break; //
    case 7:  offset = 4; addr = 3;  break; //
    case 8:  offset = 0; addr = 4;  break; //
    case 10: offset = 0; addr = 5;  break; //
    case 11: offset = 4; addr = 5;  break; //
    case 13: offset = 4; addr = 6;  break; //
    case 14: offset = 0; addr = 7;  break; //
    case 17: offset = 4; addr = 8;  break; //
    case 19: offset = 4; addr = 9;  break; //
	//----
    case 20: offset = 0; addr = 10; break; //
    case 25: offset = 4; addr = 12; break; //
    case 26: offset = 0; addr = 13; break; //
    case 28: offset = 0; addr = 14; break; //
    case 29: offset = 4; addr = 14; break; //
    case 31: offset = 4; addr = 15; break; //0x0f
    case 32: offset = 0; addr = 16; break; //0x10
	case 33: offset = 4; addr = 16; break; //0x10
	case 34: offset = 0; addr = 17; break; //0x11
    case 35: offset = 4; addr = 17; break; //0x11
	case 37: offset = 4; addr = 18; break; //0x12
    case 38: offset = 0; addr = 19; break; //0x13
    case 39: offset = 4; addr = 19; break; //0x13
	//----
	default: offset = -1; addr = -1;break; //
  }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CBTDlg::SMUX_Show(char setup, char position)
{
  CString txt;
  if(setup!=0) txt.Format("Ch%d", setup);
  else txt = "";

  switch(position)
  {
    case 2:  m_F1_2.SetWindowText(txt);  break;
    case 32: m_F1_32.SetWindowText(txt); break;
    case 10: m_F2_10.SetWindowText(txt); break;
    case 25: m_F2_25.SetWindowText(txt); break;
    case 1:  m_F3_1.SetWindowText(txt);  break;
    case 31: m_F3_31.SetWindowText(txt); break;
    case 11: m_F4_11.SetWindowText(txt); break;
    case 26: m_F4_26.SetWindowText(txt); break;
    case 13: m_F5_13.SetWindowText(txt); break;
    case 19: m_F5_19.SetWindowText(txt); break;
    case 8:  m_F6_8.SetWindowText(txt);  break;
    case 29: m_F6_29.SetWindowText(txt); break;
    case 14: m_F7_14.SetWindowText(txt); break;
    case 20: m_F7_20.SetWindowText(txt); break;
    case 7:  m_F8_7.SetWindowText(txt);  break;
    case 28: m_F8_28.SetWindowText(txt); break;
    case 39: m_FL_39.SetWindowText(txt); break;
    case 17: m_CL_17.SetWindowText(txt); break;
    case 35: m_CL_35.SetWindowText(txt); break;
    case 38: m_IR_38.SetWindowText(txt); break;

    default: return;
  }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------  
void CBTDlg::SMUX_ControlIni(void)
{
  
}

/*
//------------------------------------------------------------------------------
//контекстное меню
//------------------------------------------------------------------------------
void CBTDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
 if(point.x == -1 && point.y == -1)
 {
   //keystroke invocation
   CRect rect;
   GetClientRect(rect);
   ClientToScreen(rect);

   point = rect.TopLeft();
   point.Offset(15, 15);
 }
    
  CMenu menu;
  VERIFY(menu.LoadMenu(IDR_MENU1));
  /*
  if(DSPL.object_selected==0 &&
     DSPL.index_selected.GetSize()==0)
  {
    VERIFY(menu.LoadMenu(IDR_MENU1));
  }  
  
  else if(DSPL.index_selected.GetSize()>0)
  {
    VERIFY(menu.LoadMenu(IDR_MENU2));
  }

  //только для линий и точек
  else if(DSPL.object_selected==1 &&
    (DSPL.object_type==1 || 
     DSPL.object_type==2 || 
     DSPL.object_type==3))
  { 
    VERIFY(menu.LoadMenu(IDR_MENU1)); 
  }
  else return;
  */
/*
  //DSPL.ShowMenu(&menu);
  CMenu *pPopup = menu.GetSubMenu(0);
  ASSERT(pPopup != NULL);
  CWnd* pWndOwner = pWnd;

  //while(pWndOwner->GetStyle() & WS_CHILD)
  pWndOwner = pWndOwner->GetParent();

  pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
                         point.x, point.y, pWndOwner);	 
}
*/
//------------------------------------------------------------------------------ 
//
//------------------------------------------------------------------------------
void CBTDlg::SMUX_PopupMenu(CWnd* pWnd, CRect *pRect)
{
  //HMENU hmenu;	// шаблон меню
  //HMENU hmenuTrackPopup;	// контекстное меню
  
  //Загрузите шаблон меню, содержащий контекстное меню из
  //ресурсов программы
  CMenu menu;
  VERIFY(menu.LoadMenu(IDR_MENU1));
  if(menu == NULL) return;

  // Получим первое контекстное меню в шаблоне меню. Это -
  // то меню, которое показывает TrackPopupMenu
  CMenu *pPopup = menu.GetSubMenu(0);
  ASSERT(pPopup != NULL);

  //TrackPopup использует экранную систему координат, чтобы преобразовать
  //координаты щелчка клавишей мыши в экранную систему координат
  //ClientToScreen(hwnd, (LPPOINT) &pt);
  POINT point;
  point.x = pRect->left+20;
  point.y = pRect->top-70;

  pWnd = pWnd->GetParent();

  //Прорисовываем и устанавливаем контекстное меню
  pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, 
                         point.x, point.y, pWnd);
  //Разрушаем меню
  pPopup->DestroyMenu();
}

//------------------------------------------------------------------------------ 
//Disable all chennels
//------------------------------------------------------------------------------
void CBTDlg::OnButtonSMUX_OffAll()
{
  //выключить все каналы в GUI  
  for(char i=0; i<40; i++)
  {
	//----
	SMUX_Show(0, i);
  }

  //clear configuration 
///  memset(pDev->smux, 0, AS7341_SMUX_LEN);
  //Send cmd to main dlg
  ::PostMessageA(hWndParent, UWM_DIALOG_MESSAGES, SET_SMUX, channel);
}

//------------------------------------------------------------------------------ 
//
//------------------------------------------------------------------------------
void CBTDlg::SMUX_SetVal(char address, char shift, char value)
{
  value &= 0x07;
  TRACE2("SMUX[%d], Shift: <<%d, ", address, shift);
  if(value!=0){TRACE1("Channel: %d\n", value);}
  else{TRACE("Channel: OFF\n");}

  //=======================================
  //забираем во временный буфер 
  unsigned char data;// = pDev->smux[address];
  //очистка позиции
  data &= (0xf0<<shift);
  //вставляем данные
  data |= (value<<shift);
  //сохраняем обратно
///  pDev->smux[address] = data; 
}

//------------------------------------------------------------------------------ 
//
//-----------------------------------------------------------------------------  
char CBTDlg::SMUX_GetVal(char position)
{
  char shift, address;

  //get byte address and data offset
  SMUX_Map(position, shift, address);
  if(shift==-1 || address==-1) return 0;
  
  //читаем байт из массива
  unsigned char data;// = pDev->smux[address];
  
  //сдвигаем данные
  data = (data>>shift);

  //очистка позиции
  data &= 0x0F;

  return data;
}

//------------------------------------------------------------------------------ 
//
//------------------------------------------------------------------------------
void CBTDlg::OnButtonSMUX_Default()
{  
  //Send cmd to main dlg
  ::PostMessageA(hWndParent, UWM_DIALOG_MESSAGES, SET_SMUX, channel);
}

//------------------------------------------------------------------------------ 
//
//------------------------------------------------------------------------------
void CBTDlg::OnCheckLedOn()
{
///  pDev->led_enable = (unsigned char) m_check_led_on.GetCheck();
  //Send cmd to main dlg
  ::PostMessageA(hWndParent, UWM_DIALOG_MESSAGES, SET_LED, channel);
}

//------------------------------------------------------------------------------ 
//
//------------------------------------------------------------------------------
void CBTDlg::OnButtonOpenDoc()
{
  CString file, file_path;
 
  file = "Doc\\AS7341.pdf";

  TCHAR buffer[512];
  memset(buffer, 0, sizeof(buffer));
  int ret = GetCurrentDirectory(sizeof(buffer), buffer);
  if(ret<=0) return;
  
  CString dir(buffer);
  file_path = "rundll32.exe shell32.dll,OpenAs_RunDLL ";
  file_path += dir;
  file_path += "\\";
  file_path += file;// += "vcnl36826s AppNote.pdf";
  
  //WinExec is probably easier
  ret = WinExec(file_path, SW_SHOWDEFAULT);

  //successful
  if(ret >= 33) return;

  //error case
  //show error message
  AfxMessageBox("Error, can`t open \""+ file +"\"");
}

//==============================================================================
//File name:    "MainDlg.cpp"
//Purpose:      Source File
//Version:      1.00
//Copyright:    (c) 2022, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "stdafx.h"
#include "PIDPC.h"
#include "MainDlg.h"
#include "cmd.h"
#include "datatx.h"
#include <stdio.h>

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

extern DWORD CAN_BUS_MONITORING(LPVOID lParam);

CMainDlg *pCMainDlg = NULL;

//------------------------------------------------------------------------------
//Function:	CMainDlg dialog
//------------------------------------------------------------------------------
CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/): CDialog(CMainDlg::IDD, pParent)
{
  //утечки пам€ти - потом убрать
  //https://docs.microsoft.com/en-gb/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019
  ///_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
  //утечки пам€ти - потом убрать
	
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

  //получить €зык системы 
  //“екущий €зык интерфейса: HKEY\_LOCAL\_MACHINE\SYSTEM\CurrentControlSet\Control\MUI\UILanguages
  //Chinese (Simplified, China) 	
  //zh-CN: Microsoft Pinyin - Simple Fast 
  //(0804:{81D4E9C9-1D3B-41BC-9E6C-4B40BF79E35E}{FA550B04-5AD7-411f-A5AC-CA038EC515D7}) 	
  //Chinese - (Traditional, Taiwan) 	
  //zh-TW: Chinese (Traditional) - New Phonetic 
  //(0404:{B115690A-EA02-48D5-A231-E3578D2FDF80}{B2F9C502-1742-11D4-9790-0080C882687E})  
  //https://www.autoitscript.com/autoit3/docs/appendix/OSLangCodes.htm
  //«ащита от китайцев
  /*
  int lng_id;
  lng_id = GetSystemDefaultLangID();
  if(lng_id==0x0804 || lng_id==0x0404 || lng_id==0x0478)
  {
	 CString msg;
	 msg = "App Internal error";
	 msg += "\r\nCan`t allocation memory";
	 msg += "\r\nContact the developer for a fix";
	 msg += "\r\nhttps://en.gradient-sg.com/t962/";
     AfxMessageBox(_T(msg),  MB_ICONWARNING);

	 msg = "Unhandled exception at 0x00412b23 in: 0xC0000005";
	 msg += "\r\nAccess violation reading location 0xcccccccc";
	 AfxMessageBox(_T(msg), MB_ICONSTOP);
	 
	 //«акрыть приложение
	 CDialog::OnCancel();
  }	*/
  
  pBTDlg = NULL;

  //Allocate memory for objects
  pPage1 = new CPage1;	
  pPage2 = new CPage2;
  pPage3 = new CPage3;
  pPage4 = new CPage4;
  pPage5 = new CPage5;
  pPage6 = new CPage6;
  
  pCMainDlg = this;
  pDevice = &Device;
  memset(&Device, 0, sizeof(Device_Setup));
  memset(&Params, 0, sizeof(Device_Params));
  
  //----
  memset(&APP, 0, sizeof(ApplSettings));
  APP.struct_length = sizeof(ApplSettings);
  CSETUP.pSetup = (ApplSettings*)&APP;
  CSETUP.hREG = (HANDLE)&REG;
  
  //----
  pPage1->pBT = &BT;
  pPage2->pBT = &BT;
  pPage3->pBT = &BT;
  pPage4->pBT = &BT;
  pPage5->pBT = &BT;  
  pPage6->pBT = &BT;
  
  //ble
//  pPage1->pBLE = &BLE;
  pPage2->pBLE = &BLE;
//  pPage3->pBLE = &BLE;
  pPage4->pBLE = &BLE;
//  pPage5->pBLE = &BLE;
  pPage6->pBLE = &BLE;

  pPage6->pCSETUP = &CSETUP;

  pPage1->pDevice = &DEV;
  pPage2->pDevice = &DEV;
  pPage3->pDevice = &DEV;
  pPage4->pDevice = &DEV;
  pPage5->pDevice = &DEV;
//  pPage6->pDevice = &DEV;

  //----
  pPage1->pParams = &Params;  
//  pPage2->pParams = &Params;
  pPage3->pParams = &Params;
//  pPage4->pParams = &Params;
//  pPage5->pParams = &Params;
//  pPage6->pParams = &Params;

  //----
//  pPage1->pAPP = &APP; 
//  pPage2->pAPP = &APP;
//  pPage3->pAPP = &APP; 
//  pPage4->pAPP = &APP;   
  pPage5->pAPP = &APP; 
  pPage6->pAPP = &APP;

  //----
  pPage1->pREG = &REG;
  pPage2->pREG = &REG;
  pPage3->pREG = &REG;
  pPage4->pREG = &REG;
  pPage5->pREG = &REG;
  pPage6->pREG = &REG;

  //----
//  pPage1->pConsole = &Console;
  pPage2->pConsole = &Console;
//  pPage3->pConsole = &Console;
  pPage4->pConsole = &Console;
//  pPage5->pConsole = &Console;
  pPage6->pConsole = &Console;

  //----
  pPage4->pPage2 = pPage2;

  //clear pointers
  pThread = NULL;
  monitoring_ack = 0;
  exit_request = 0;
  tab_opened = 0;
  hTimer = 0;
  Console.SetWindowSize(100, 40);
}

//------------------------------------------------------------------------------
//Function:	
//------------------------------------------------------------------------------
CMainDlg::~CMainDlg()
{
  if(hTimer!=0)KillTimer(hTimer);
  
  BLE.Disconnect();
  BLE.Close();

  ThreadStop();
  
  //ждем завершени€ цикла, таймаут если данных нет
  //WaitForSingleObject(ev_WaitEnd.m_hObject, 10000);

  //Free objects memory   
  delete pPage6;
  pPage6 = NULL;
  delete pPage5;
  pPage5 = NULL;  
  delete pPage4;
  pPage4 = NULL;
  delete pPage3;
  pPage3 = NULL;
  delete pPage2;
  pPage2 = NULL;
  delete pPage1;
  pPage1 = NULL;

  DeleteObject(hLED_grey);
  DeleteObject(hLED_green);   
  hLED_grey = NULL;
  hLED_green = NULL;

  //_CrtDumpMemoryLeaks();
}

//------------------------------------------------------------------------------
//Function:	
//------------------------------------------------------------------------------
void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//----radio
	DDX_Control(pDX, IDC_TAB1, m_Tab);
	DDX_Control(pDX, IDC_COMBO_DEVICE_LIST, m_combo_devices_list);	
	DDX_Control(pDX, IDC_STATIC_DEVICE_LED, m_led);
	DDX_Control(pDX, IDC_STATIC_DEVICE_TXT, m_led_txt);
	DDX_Control(pDX, IDC_BUTTON_READ_SETUP, m_but_GetSetup);
	DDX_Control(pDX, IDC_BUTTON_CONSOLE_CLS, m_but_LogClear);
	DDX_Control(pDX, IDC_BUTTON_RESET, m_but_DevReset);
	DDX_Control(pDX, IDC_BUTTON_GET_DEVICE_LIST, m_but_DevList);
	DDX_Control(pDX, IDC_COMBO_BT, m_combo_bt_adapters);
}

//------------------------------------------------------------------------------
//Function:	message handlers
//------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
  ON_WM_SYSCOMMAND()
  ON_WM_CLOSE()
  ON_WM_PAINT()
  ON_WM_TIMER()
  ON_WM_DEVICECHANGE()
  ON_WM_QUERYDRAGICON()
  ON_WM_WINDOWPOSCHANGED()
  ON_MESSAGE(UWM_DIALOG_MESSAGES, &CMainDlg::OnDlgMessges)
  ///ON_COMMAND(ID_HELP, &CMainDlg::HtmlHelp)
  //----
  ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnTcnSelchangeTab)
  ON_NOTIFY(TCN_SELCHANGING, IDC_TAB1, OnTcnSelchangingTab)
  //----
///  ON_LBN_SELCHANGE(IDC_MAIN_DEVICE_LIST, OnSelchangeDeviceList)
  ON_BN_CLICKED(IDC_BUTTON_UPDATE, &CMainDlg::OnButtonFirmWareUpdate)
///  ON_LBN_DBLCLK(IDC_MAIN_DEVICE_LIST, &CMainDlg::OnDoubleClickDeviceList)
  ON_BN_CLICKED(IDC_BUTTON_CAN, &CMainDlg::OnButtonCanSetup)
  //----
  ON_BN_CLICKED(IDC_BUTTON_READ_SETUP, &CMainDlg::OnButtonGetSetup)
  //----
  ON_BN_CLICKED(IDC_BUTTON_RESET, &CMainDlg::OnButtonOvenReset)
  ON_BN_CLICKED(IDC_BUTTON_CONSOLE_CLS, &CMainDlg::OnButtonConsoleCls)
  //----
///  ON_CBN_SELCHANGE(IDC_COMBO_DEVICE, &CMainDlg::OnSelchangeDevice)

  ON_BN_CLICKED(IDC_BUTTON_GET_DEVICE_LIST, &CMainDlg::OnButtonDeviceList)
  ON_CBN_CLOSEUP(IDC_COMBO_DEVICE_LIST, &CMainDlg::OnCbnCloseupComboDeviceList)
  ON_CBN_SELCHANGE(IDC_COMBO_BT, &CMainDlg::OnSelchangeBTAdapters)
  ON_BN_CLICKED(IDC_BUTTON_BT, &CMainDlg::OnButtonBT)
END_MESSAGE_MAP()

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
BOOL CMainDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  //Set the icon for this dialog.  The framework does this automatically
  //when the application's main window is not a dialog
  SetIcon(m_hIcon, TRUE);			// Set big icon
  SetIcon(m_hIcon, FALSE);		// Set small icon
  //TODO: Add extra initialization here

  //Preload LED images
  HINSTANCE hInstance;
  hInstance = AfxGetResourceHandle();
  hLED_red = LoadBitmapA(hInstance, (LPCSTR)IDB_LED_RED);
  hLED_grey = LoadBitmapA(hInstance, (LPCSTR)IDB_LED_GREY);
  hLED_green = LoadBitmapA(hInstance, (LPCSTR)IDB_LED_GREEN);
  
  // TODO: Add extra initialization here
  if(REG.FirstLaunch()==1)
  {
    //First ini	
	REG.SetIntVar("btvid", 1);
	REG.SetIntVar("btpid", 1);
	REG.SetIntVar("bt_custom", 0);

	CString str = "";
	REG.SetTxtVar("btchip", str);
	str = "";
	REG.SetTxtVar("btman", str);
	str = "";
	REG.SetTxtVar("btfw", str);
  }

  //прочитать настройки приложени€
  CSETUP.Read();
  LED.Ini(this->m_hWnd, &m_led, &m_led_txt, hLED_grey, hLED_green, hLED_red);

  //Create and initialisation Tab Pages
  #include "pages.h"
  
  POS.SetWindowPositon(this->m_hWnd); //GetSafeHwnd()

  //---- Set Last Open Tab page
  //REG.SaveLastTab(0);
  int tab; 
  REG.GetIntVar("last_tab", tab);
  m_Tab.SetCurSel(tab);
  
  switch(tab)
  {
    case 0: pPage1->ShowWindow(SW_SHOW); break;
    case 1: pPage2->ShowWindow(SW_SHOW); break;  
  	case 2: pPage3->ShowWindow(SW_SHOW); break;
    case 3: pPage4->ShowWindow(SW_SHOW); break;
    case 4: pPage5->ShowWindow(SW_SHOW); break;
    case 5: pPage6->ShowWindow(SW_SHOW); break;
  }
  //current tab
  tab_opened = tab;

  //---- PID_channel
  m_combo_devices_list.EnableWindow(TRUE);
  LED_Control(0); 

  pPage2->Controls_Ini();

  //==========================================================
  //Set the tooltip (help hints)
  //==========================================================
  struct prj_tooltip tooltips[] = 
  { 
    
	{&m_but_DevList, "Update device list"},  
	{&m_combo_devices_list, "Select remote device"},
    {&m_but_GetSetup, "Get Oven Settings"},
//	{&m_but_SetupDefault, "Set All Device Settings to Default"},
//    {&m_but_FirmwareUpdate, "Device Firmware Update"},
	{&m_but_LogClear, "Clear Console log"},
	{&m_but_DevReset, "Urgent Oven Reset"},
//	{&m_but_CAN, "BT adapter setup"}
  };

  ToolTip.SetToolTip(FromHandle(this->m_hWnd), tooltips, _countof(tooltips));
  
  //monitoring thread 
  ThreadLaunch();
  
  
  BLE_ini();  
  BLE_Open();
  OpenCommunication();
  
  //return TRUE  unless you set the focus to a control
  return TRUE;  
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CMainDlg::OnTimer(UINT_PTR nIDEvent)
{
  if(nIDEvent==ID_TIMER_GET_DATA)
  {	 
    /////CAN_SendCommand(pCAN,CAN_device_ID, 0, CMD_MONITORING_ON, 0, 0);
  }	

  if(nIDEvent==ID_TIMER_GET_DEVICE)
  {
  	KillTimer(hTimer);
	hTimer = 0;
	OnSelchangeDeviceList();
  }
  
  if(nIDEvent==ID_TIMER_MONITORING)
  {
  	KillTimer(hTimer1);
	hTimer1 = 0;
/*	if(CAN_device_ID!=0)
	{
	  //Launch device monitorng
	  Monitoring.Enable(CAN_device_ID);
	} */
  }

  //do not delete this string!
  CDialog::OnTimer(nIDEvent);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CMainDlg::TimerControl(int period)
{
 if(hTimer==0)
  {	
	//---------------------------------------
    hTimer = SetTimer(ID_TIMER_GET_DATA, period, 0); 
   }
  else 
  {
    KillTimer(hTimer);
	hTimer = 0;
  }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int CMainDlg::OpenCommunication(void)
{
  //Check already opened
  //if(pCAN!=NULL) return 1;
  //info
  m_combo_devices_list.Clear();
  //m_devices_list.AddString("Wait.. initialization");
  m_combo_devices_list.AddString("Wait..");
  //m_devices_list.AddString("T-962");
  //m_devices_list.AddString("No BT adapters");
  m_combo_devices_list.SetCurSel(0);

  //==============================================
  //---- CAN
  //==============================================  
  //Open CAN Port
  int result = -1;/// CAN_Open(&pCAN, "CAN", 500); 
    
  //Check
  if(result<0) return 0;
    
 /// USBCAN_SetHotPlugUSB(pCAN, this->m_hWnd, NULL);

  //Translate pointer to all pages
  Controls_Enable(FALSE);
  
  //Get devices list
  hTimer = SetTimer(ID_TIMER_GET_DEVICE, 1000, 0); 

  return 1;  
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CMainDlg::Controls_Enable(BOOL on_off)
{
  //List control`s
  //m_devices_list.EnableWindow(on_off);  
  pPage1->Controls_Enable(on_off);
  pPage2->Controls_Enable(on_off);
  pPage3->Controls_Enable(on_off);
  pPage4->Controls_Enable(on_off);
  pPage5->Controls_Enable(on_off);
  pPage6->Controls_Enable(on_off);

  ///m_but_FirmwareUpdate.EnableWindow(on_off);
  ///m_but_DevList.EnableWindow(on_off);
  m_but_GetSetup.EnableWindow(on_off);
  m_but_DevReset.EnableWindow(on_off);

///  if(on_off) hTimer1 = SetTimer(ID_TIMER_MONITORING, 500, 0); 
}

//------------------------------------------------------------------------------
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
//------------------------------------------------------------------------------
void CMainDlg::OnPaint()
{
 if(IsIconic())
  {	 
    //device context for painting
    CPaintDC dc(this); 
    SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

    //Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    //Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  }
 else{CDialog::OnPaint();}
}

//------------------------------------------------------------------------------
// The system calls this function to obtain the cursor to display while the 
// user drags the minimized window.
//------------------------------------------------------------------------------
HCURSOR CMainDlg::OnQueryDragIcon()
{
  return static_cast<HCURSOR>(m_hIcon);
}

//------------------------------------------------------------------------------
//Handler: page change    (rus: обработчик смены закладок)
//------------------------------------------------------------------------------
#pragma warning(disable : 4100)
void CMainDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{	
  int iTab = m_Tab.GetCurSel();
  TC_ITEM tci;
  tci.mask = TCIF_PARAM;
  m_Tab.GetItem(iTab, &tci);
  CWnd* pWnd = (CWnd *)tci.lParam;
  pWnd->ShowWindow(SW_SHOW); 
  
  //Save current Tab number  
  REG.SetIntVar("last_tab", iTab);

  //Do it, if this page is open 
  switch (iTab)
   {	
     //Open Tab Page1
     case 0:
     //Launch device monitorng
	 pPage1->Monitoring(1);
     break;
        
     //Open Tab Page2
     case 1:
     //Launch device monitorng	
     break;

     //Open Tab Page3
     case 2:
     //Launch device monitorng
     break;
     
	 //Open Tab Page4
     case 3:
     pPage4->LoadProfile();
     break;
                                                    
     default:
     break;
  };

  tab_opened = iTab;
  
  //Do not delete this string
 *pResult = 0;
}

//-----------------------------------------------------------------------------
//handler  page change  обработчик скрыти€ закладки
//-----------------------------------------------------------------------------
void CMainDlg::OnTcnSelchangingTab(NMHDR *pNMHDR, LRESULT *pResult)
{	
  int iTab = m_Tab.GetCurSel();
  TC_ITEM tci;
  tci.mask = TCIF_PARAM;
  m_Tab.GetItem(iTab, &tci);
  CWnd* pWnd = (CWnd *)tci.lParam;
  pWnd->ShowWindow(SW_HIDE); 
    
  //Do it, if this page is closed 
  switch (iTab)
   { 
     case 0:
	 pPage1->Monitoring(0);
     break;

     case 2:
//	 pPage4->Monitoring(0);
     break;

     default:
     break;
   };	
  
  tab_opened = iTab;   
 *pResult = 0;
}
#pragma warning(push)
/*
//------------------------------------------------------------------------------
//Function: 
//Here analysis correct data received, CRC
//------------------------------------------------------------------------------
unsigned char CRC8(void *buffer_pointer, unsigned short size)
{
  
  unsigned char temp = 0; 
  unsigned short   i = 0;
  unsigned char *pBuffer = (unsigned char*)buffer_pointer;
    
  //----- проверить целостность протокола
  do {
        temp += *pBuffer++;
        i++;
     }
  while (i<size);

  //возвращаем crc
  return ~temp;
} */

//-----------------------------------------------------------------------------
//Function: запрос к устройству предоставить настройки
//-----------------------------------------------------------------------------
int CMainDlg::GetDeviceSetup(void)
{
  return BT.Tx(CMD_GET_DEVICE_SETUP, NULL, 0);
}

 /* 
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int CMainDlg::GetDeviceSetup(void)
{ 	  
  int res;
  unsigned char cmd;
  unsigned short length, setup_length;

  //TRACE("GetSetupLength\n");
  //DLL
  cmd = CMD_GET_SETUP_LENGTH;
  res = CAN_ReadDevice(pCAN, device_id, CAN_CMD_CHANNEL, 
                   (unsigned char*)&length, 2, &cmd, 1, 100);
 */
  //TRACE2("GetSetupLength ID:%d  Ret:%d\n", device_id, res);
  /*  
  //провер€ем размер местной структуры чтоб было совпадение
  setup_length = sizeof(Device_Setup);
  if(setup_length!=length)
  {

   //выдел€ем пам€ть дл€ считвыани€ сетап
   char *pMem = new char [length];
   if(pMem==NULL) return -1;

   //DLL
   cmd = CMD_GET_DEVICE_SETUP;
   memset(pDevice, 0, sizeof(Device_Setup));
	 
   //TRACE("GetDeviceSetup\n");
   //---- ask board
   res = CAN_ReadDevice(pCAN, device_id, CAN_CMD_CHANNEL,
                       (unsigned char*)pMem, length, &cmd, 1, 600);

   if(res<0){ if(pMem!=NULL)delete [] pMem; return -1;}

   //Device_type
   char device_type = pMem[3];
   if(pMem!=NULL)delete [] pMem;
   if(device_type!=DEVICE_TECv2_CONTROLLER)
   {
  	 AfxMessageBox("Error: Device have old version, it no supported here");
  	 return -1;
   }  
  }
 */
  /*
  //провер€ем размер местной структуры чтоб было совпадение
  setup_length = sizeof(Device_Setup);
  if(length!=setup_length)
  {
  	 AfxMessageBox("Error: Device Setup length error");
  	 return -1;
  }*/
 /*
     //DLL
     cmd = CMD_GET_DEVICE_SETUP;
     memset(pDevice, 0, sizeof(Device_Setup));
	 
	 //TRACE("GetDeviceSetup\n");
     //---- ask board
     res = CAN_ReadDevice(pCAN, device_id, CAN_CMD_CHANNEL,
                             (unsigned char*)pDevice, length, &cmd, 1, 600);
	 //TRACE2("GetDeviceSetup ID:%d  Ret:%d\n", device_id, res);
  
  return res;  
}			   */

//------------------------------------------------------------------------------
//Function:
//------------------------------------------------------------------------------
void CMainDlg::OnDoubleClickDeviceList()
{
  OnSelchangeDeviceList();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CMainDlg::OnButtonDeviceList()
{ /*
  //Get sensors device list
  CAN_GetDeviceListCombo(pCAN, &m_devices_list, 
                         CANID_FIRST, CANID_LAST, NULL, NULL);  
	
  int result = CAN_ReturnDeviceCount(pCAN);
  if(result<1)
  {
     Controls_Enable(0);
	 LED_Control(0);
     return;
  } 
*/
  m_combo_devices_list.ShowDropDown(1);
}

//------------------------------------------------------------------------------
//Function:
//------------------------------------------------------------------------------
void CMainDlg::OnCbnCloseupComboDeviceList()
{
  int index = m_combo_devices_list.GetCurSel();
  BLE_SelchangeDevice(index);
}

//------------------------------------------------------------------------------
//Function:
//------------------------------------------------------------------------------
void CMainDlg::OnSelchangeDeviceList()
{
  int index = m_combo_devices_list.GetCurSel();
  BLE_SelchangeDevice(index);
}

//-----------------------------------------------------------------------------
//Parse Current Car Pin Status
//-----------------------------------------------------------------------------  
void CMainDlg::DeviceList(int param)
{		  
	
	
  /*
  //Get sensors device list
  CAN_GetDeviceListCombo(pCAN, &m_devices_list, 
                         CANID_FIRST, CANID_LAST, NULL, NULL);  
					  */
  int result = -1; /// CAN_ReturnDeviceCount(pCAN);


  if(result<1)
  {
     //no devices
	 Controls_Enable(0);
	 LED_Control(0);
     return;
  } 

  int current_device;
  if(param<0)
  {
    current_device = m_combo_devices_list.GetCurSel(); 
    if(current_device<0 || result<(current_device+1))
    {
      //TEST
///      REG.SetIntVar("last_board_id", CAN_device_ID);
      Controls_Enable(1);
      return;
    }
  }
  else {current_device = param;}
   
  //Get new data
  result = -1; /// CAN_ReturnDeviceID(pCAN, current_device);
  if(result<0 ){Controls_Enable(0); return;} 
  
  unsigned char device_ID = (unsigned char)result; 

  if(device_ID>=CANID_FIRST&&device_ID<=CANID_LAST)
   { 
///     CAN_device_ID = device_ID;

     //translate ID
     //read data from device 	
     result = 0; ///GetDeviceSetup();
     if(result==1)
     {
///       REG.SetIntVar("last_board_id", CAN_device_ID);
       Controls_Enable(1);
       LED_Control(1);
       //----
       pPage1->Controls_Update(0);
       pPage2->Controls_Update(0);
	   pPage3->Controls_Update(0);
	   pPage4->Controls_Update(0);
	   pPage5->Controls_Update(0);
	   pPage6->Controls_Update(0);           
     }
     else{Controls_Enable(0); /*pPage2->Monitoring_Channel(0);*/}
   }
}

//-----------------------------------------------------------------------------
//Parse Current Car Pin Status
//-----------------------------------------------------------------------------
void CMainDlg::LED_Control(int online)
{ 
 if(online==1)
 {
   m_led.SetBitmap(hLED_green);
   m_led_txt.SetWindowTextA(_T("Connected"));
   pPage4->device_online = 1;
 } 
 else 
 {
   m_led.SetBitmap(hLED_grey);
   m_led_txt.SetWindowTextA(_T("Disconnected"));
   pPage4->device_online = 0;
 }

  m_led.ShowWindow(SW_SHOW);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
  ToolTip.PreTranslateMessage(pMsg);
  return CDialog::PreTranslateMessage(pMsg);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
///void CMainDlg::OnButtonCanSetup(){CAN_Setup_Dialog(GetSafeHwnd(), pCAN);}

void CMainDlg::OnButtonCanSetup(){ /*OpenBLE();*/}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CMainDlg::OnButtonFirmWareUpdate()
{
  //device monitorng stop
  //isp
///  CAN_ISP_Dialog(GetSafeHwnd(), pCAN);

//  LED.Blink(pPage4->device_online, 3, 80, "Save..");
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void CMainDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
  CDialog::OnWindowPosChanged(lpwndpos);
  POS.WindowPosChanged(this->m_hWnd, lpwndpos);
}

//-----------------------------------------------------------------------------
//USB Hotplug callback
//-----------------------------------------------------------------------------
BOOL CMainDlg::OnDeviceChange(UINT EventType, DWORD_PTR dwData)
{
  int result = 0; /// USBCAN_HotPlugHandler(pCAN, EventType, dwData);
  if(result==1)
    {
      Controls_Enable(TRUE);
      LED_Control(1);
      Sleep(1000);
      result = 0;//GetDeviceSetup();
      
      if(result==1)
       {
         pPage1->Controls_Update(0);
         pPage1->Controls_Enable(TRUE);
         //----
		 pPage2->Controls_Update(0);
         pPage2->Controls_Enable(TRUE);
         //----
		 pPage3->Controls_Update(0);
         pPage3->Controls_Enable(TRUE);
         //----
		 pPage4->Controls_Update(0);
         pPage4->Controls_Enable(TRUE);
         //----
         pPage5->Controls_Update(0);
         pPage5->Controls_Enable(TRUE);
         //----
         pPage5->Controls_Update(0);
         pPage5->Controls_Enable(TRUE);
         //----
         pPage6->Controls_Update(0);
         pPage6->Controls_Enable(TRUE);
       }
      return TRUE;
    }

  if(result==-1)
   { 
     LED_Control(0);
     Controls_Enable(FALSE);
     //pPage2->Monitoring_Channel(0);
   } 

  return TRUE;
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void CMainDlg::OnButtonGetSetup()
{
  //read data from device 	
  GetDeviceSetup();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void CMainDlg::OnButtonOvenReset()
{
  //Tx
  BT.Tx(CMD_NRF_OVEN_RESET, NULL, 0);
  //BT.Tx(CMD_NRF_OVEN_PROG_MODE, NULL, 0);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void CMainDlg::OnButtonConsoleCls()
{
  ///if(Console.opened==0)Console.Open("Device Log"); 
  if(Console.opened==1) Console.Clear();
  pPage6->Clear();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CMainDlg::OnSelchangeDevice()
{ /*
  unsigned char device_ID = 0;
  
  unsigned char index;
  index = (unsigned char) m_devices_list.GetCurSel();

  //Get data
  int result = -1; ///CAN_ReturnDeviceID(pCAN, index);
  if(result<0 ){Controls_Enable(0); return;} 
  
  device_ID = (unsigned char)result;
  if(device_ID==0) return;
	   
  if(device_ID>=CANID_FIRST&&device_ID<=CANID_LAST)
   { 
///     CAN_device_ID = device_ID;

     //translate ID

     //read data from device 	
     int result = 0;///GetDeviceSetup();
     if(result==1)
     {
///       REG.SetIntVar("last_board_id", CAN_device_ID);
       Controls_Enable(1);
       LED_Control(1);
       //----
       pPage1->Controls_Update(0);
       pPage2->Controls_Update(0);
	   pPage3->Controls_Update(0);
	   pPage4->Controls_Update(0);
	   pPage5->Controls_Update(0);
 	   pPage6->Controls_Update(0);
       //pPage2->Monitoring_Channel(1);             
     }
     else{Controls_Enable(0); /*pPage2->Monitoring_Channel(0);*//*}
   }
  */
}
	
extern int boot_mode;

//------------------------------------------------------------------------------
//Function:
//------------------------------------------------------------------------------ 
/*
тут надо поставить 2 независимых декодера
*/
void CMainDlg::DataRx(char *pBuf, int length)
{ /*
  debug_byte_count += length;
  if(transport==TRANSPORT_COM) TRACE2("Read COM %d Cnt %d\n", length, debug_byte_count);
  else TRACE2("Read BLE %d Cnt %d\n", length, debug_byte_count);
  */

  //----
  for(int i=0; i<length; i++)
  {
	//Search data block header 
    int ready = PROTOCOL.Decoder(pBuf[i]);
  	if(ready==1)
	{
	  //Data Block was accepted
	  DeviceMsgHandler(PROTOCOL.cmd, PROTOCOL.pDataBuf, PROTOCOL.data_length);
	  PROTOCOL.FreeMem();
	  //reset progress
	  ///m_progress.SetPos(0);
      ///m_progress.SetBkColor(RGB(200, 200, 200));
	}

	//if protocol data header was accepted 
    if(PROTOCOL.header_accepted==1)
	{
	  //ini progress
	  ///m_progress.SetRange(0, PROTOCOL.data_length); //from to
	  ///m_progress.SetBkColor(RGB(255,255,255));
	  ///m_progress.SetPos(0);
	  //clear flag
	  PROTOCOL.header_accepted = 0;
	}
	
	//progress stepping
	///if(PROTOCOL.data_counter!=0) 
	   ///m_progress.SetPos(PROTOCOL.data_counter); 

  }
}

//-----------------------------------------------------------------------------
//Function: читать структуру настроек
//-----------------------------------------------------------------------------
int CMainDlg::ReadDeviceSetup(char *pBuf, int length)
{
  int stuct_length = sizeof(Device_Setup2);
  if(stuct_length!=length)
  {
  	CString msg = "Device settings do not match\r\n"; 
	msg += "with current PC software version.\r\n";
	msg += "Check for device firmware updates.\r\n";
    msg += "Settings will be to Factory Restore.\r\n";
	msg += "Functionality can be partially damaged.";
	::AfxMessageBox(msg);
	return -1;
  }
	
  //Device_Setup buffer
  memcpy(&DEV, pBuf, stuct_length);
  return 1;
}

//------------------------------------------------------------------------------
//Parse additional parameters for command options 
//------------------------------------------------------------------------------
void CMainDlg::Print_MemBuffer(char *pBuf, int length)
{
  CString txt;
  std::string std_txt;
  long i, j;
  unsigned char data;
  
  //----
  unsigned long address = (unsigned char)*pBuf++;
  address <<= 8;
  address |= (unsigned char)*pBuf++;
  
  //----
  unsigned short page_size = (unsigned char)*pBuf++;
  page_size <<= 8;
  page_size |= (unsigned char)*pBuf++;
  
  if(page_size>length) page_size = 128;
  
  /*
  printf("\nADDR:\t");
  Console.Write("%s", "\nADDR:\t");
 
  //Print Address Header
  for(i=0; i<16; i++)
  {
	if(i==8) Console.Write("%s","| "); 
	Console.Write("%02lX ", i);
  }
  Console.Write("%s", "\n"); 
  
  for(i=0; i<57; i++) Console.Write("%s", "-");
  Console.Write("%s", "\n"); 
*/

  // pres vsechny 16-Bytove bloky
  for (i=0; i<page_size; i+=16) 
  {
    //Print Address
    ///Console.Write("%06lX: ", i+address);
	txt.Format("%06lX: ", i+address);
	std_txt = txt.GetBuffer();
	pPage6->PrintInfo(&std_txt);

    //pres vsechny byte v 16B bloku
    for(j=0; j<16; j++)               
    {
      //Print div by 8 Bytes
      if(j==8)
	  { 
	     ///Console.Write("%s", "| ");
	     txt.Format("%s", "| ");
	     std_txt = txt.GetBuffer();
	     pPage6->PrintInfo(&std_txt);
	  }
    
    if((j+i)<page_size)
    {                 
      //pokud nejsou vypsany vsechny Byte
      data = pBuf[j+i]; 
      ///Console.Write("%02X ", data);
	  txt.Format("%02X ", data);
	  std_txt = txt.GetBuffer();
	  pPage6->PrintInfo(&std_txt);

    }
    else
	{
	  //jinak misto nej vypis mezery							  
      ///Console.Write("%s", "   ");
 	  txt.Format("%s", "   ");
	  std_txt = txt.GetBuffer();
	  pPage6->PrintInfo(&std_txt);
	}
  }
   
  // konec radky
  ///Console.Write("%s", "\n");
  txt.Format("%s", "\n");
  std_txt = txt.GetBuffer();
  pPage6->PrintInfo(&std_txt);
 }
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void CMainDlg::ParseDeviceEvents(char *pBuf, int length)
{
  char buf[16];
  memcpy(buf, pBuf, sizeof(buf));

  unsigned char event_code = buf[0];

  switch(event_code)
  {
    //----
    case EVT_REFLOW_DONE:
    pPage4->Parser(pBuf);
	break;

    //----
    case EVT_CMD_ACK:
	LED.Blink(pPage4->device_online, 1, 160, NULL);
	break;

    //----
    case EVT_SAVE_EEPROM:
	LED.Blink(pPage4->device_online,  3, 80, "Save..");
	break;

	//----
	case EVT_WRITE_PROFILE:
	pPage4->block_writed = 1;
	break;

	//----
	case EVT_BOOT_MODE:
	//boot_mode = 1;
	break;

  }
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void CMainDlg::DeviceMsgHandler(char cmd, char *pBuf, int length)
{
  unsigned char command = (unsigned char)cmd;
  std::string txt;
  int result;
  unsigned char cmd_temp;

  //----
  switch(command)
  {	
    //----
    case CMD_GET_DEVICE_SETUP:
         result = ReadDeviceSetup(pBuf, length);
		 if(result>0)
		 {
		   //Device connected		   
		   //ќбновить все параметры
           pPage1->Controls_Update(0);
           pPage2->Controls_Update(0);
	       pPage3->Controls_Update(0);
	       pPage4->Controls_Update(0);
	       pPage5->Controls_Update(0);
		   pPage6->Controls_Update(0);
	       //----
		   LED_Control(1);
		   Controls_Enable(1);
		 }
     break;

	//----
	case CMD_DEV_SEND_TXT:
		 for(int i=0; i<length; i++)
		 {
		    txt.push_back(*pBuf++);
		 }
		
	     //TRACE1("%s", t);  
		 //Console.Write("%s", txt.c_str());
		 pPage6->PrintInfo(&txt);
         break;
  	
    //----
	case CMD_BOOT_SEND_TXT:
		 pPage6->ParseBootMsg(pBuf, length);
         break;
		 
	
    //----
	case CMD_DEV_SEND_EVENT:
		 ParseDeviceEvents(pBuf, length);
         break;

	//----
	case CMD_DEV_SEND_REFLOW_LOG:
		 pPage4->ParseReflowLog(pBuf, length);
		 pPage4->LogFile_Reflow(pBuf);
         break;

	//----
	case CMD_CHECK_PROFILE_CRC:
		 pPage4->CheckProfileCRC(pBuf);
         break;

	//----
	case CMD_DEV_SEND_REFLOW_STEP:
		 pPage4->ParseReflowStep(pBuf, length);
         break;

	//----
    case CMD_GET_EEPROM:
		 Print_MemBuffer(pBuf, length);
    break;			
			
	//----
    case CMD_GET_TEMPERATURE:
		 cmd_temp = pBuf[0];
		 //Page1
		 if(cmd_temp==TC_HEATER_ARRAY)
		    pPage1->HeaterTemperature(&pBuf[1]);
		 //Page4
		 if(cmd_temp==TC_HEATER)
		    pPage3->HeaterParser(&pBuf[1]);
    break;
 
    //----
    case CMD_GET_PROFILE:
		 pPage4->ParseProfile(pBuf, length); 
    break;
/*
    //----
    case CMD_FILESYS_INFO:
		 ReadFileSysInfo(pBuf, length);
    return;
    
	//----
    case CMD_DEVICE_MSG:
		 ReadDeviceMsg(pBuf, length);
    return;	*/
  }
}


void CMainDlg::OnButtonBT()
{
  //----
  if(pBTDlg==NULL) 
  { 
    unsigned char cmd = CMD_GET_DEVICE_SETUP;
	short length = sizeof(Device_Setup);
    
	//Open Dlg
    pBTDlg = new CBTDlg();
	//pBTDlg->pDev = 0;
    pBTDlg->pREG = &REG;
    pBTDlg->dev_connected = 1;//sensor_opened;
	//pBTDlg->channel = chn;
	//pBTDlg->channel_name = PARSER.ChnNameFromChnNumber(chn, DEV.Device_index);
	pBTDlg->hWndParent = this->m_hWnd;
	//pSnDlg[nRow]->Create(IDD_SENSOR_SETUP_DIALOG, this->GetOwner()); //this->GetWindow(IDD_Page8));
	pBTDlg->Create(IDD_DIALOG1, this->GetWindow(IDD_DIALOG1));
    pBTDlg->ShowWindow(SW_SHOW);
  }
}

//-----------------------------------------------------------------------------
//Function:
//-----------------------------------------------------------------------------
LRESULT CMainDlg::OnDlgMessges(WPARAM wParam, LPARAM lParam)
{
  CString txt;
  char old_scan = 0;
  long time;
  unsigned char chn = (unsigned char)lParam;  
  unsigned char data[4];
  
  //----
  switch(wParam)
  {
	  //----
      case DLG_CLOSE:
	  delete [] pBTDlg;
	  pBTDlg = NULL;
      return 1;

	  //----
      case SET_ATIME:

      break;
	
	  //----
      case SET_ASTEP:

      break;	
     
      //----
      case SET_GAIN:

	  //----

      break;
     
      //---- 
      case SET_CALIBR:


      break;

      //----
      case SET_SMUX:  	  
	  //----

      break;
  }
		
  return 1;
}


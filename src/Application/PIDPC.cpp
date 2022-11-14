//==============================================================================
//File name:    "PIDPC.cpp"
//Purpose:      Source File
//Version:      1.00
//Copyright:    (c) 2022, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "stdafx.h"
#include "PIDPC.h"
#include "MainDlg.h"

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

// CPIDPCApp
BEGIN_MESSAGE_MAP(CPIDPCApp, CWinAppEx)
ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
///ON_COMMAND(ID_HELP, &CWinApp::HtmlHelp)
ON_COMMAND(ID_CONTEXT_HELP, &CPIDPCApp::OnContextHelp)
END_MESSAGE_MAP()

// CPIDPCApp construction
CPIDPCApp::CPIDPCApp()
{
  //TODO: add construction code here,
  //Place all significant initialization in InitInstance
  ///EnableHtmlHelp();
}

// The one and only CPIDPCApp object
CPIDPCApp theApp;

//------------------------------------------------------------------------------
//Function:	CPIDPCApp initialization
//------------------------------------------------------------------------------
BOOL CPIDPCApp::InitInstance()
{
	CWinAppEx::InitInstance();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
  /*
  //Защита от китайцев
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

	 msg = "Unhandled exception at 0x0021fb0c in: 0xD0000000";
	 msg += "\r\nAccess violation reading location 0xcccccccc";
	 AfxMessageBox(_T(msg), MB_ICONSTOP);
	 
	 //Закрыть приложение
	 return FALSE;
  }	  */


	CAppWinReg_ini;
	CAppWinReg REG;
	if(REG.FirstLaunch()==1)
     {
	   REG.SetIntVar("last_tab", 5);
	   REG.SetIntVar("page1_logs", 0);
	   REG.SetIntVar("page4_live", 0);
	   REG.SetIntVar("page4_man", 0);
	   //----
	   REG.SetIntVar("page5_log", 1);
	   REG.SetIntVar("page5_info", 1);
	   //----
	   REG.SetIntVar("page8_number", 70);
	   REG.SetIntVar("page8_delay", 200);
	   REG.SetIntVar("page8_period", 5000);
	   REG.SetIntVar("page8_on_time", 800);
	   REG.SetIntVar("page8_led_mux", 0);
	   CString txt("Select file patch");
	   REG.SetTxtVar("page8_path", txt);
     } 

	CMainDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		// dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		// dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	// application, rather than start the application's message pump.
	return FALSE;
}

//------------------------------------------------------------------------------
//Function:	
//------------------------------------------------------------------------------
void CPIDPCApp::OnContextHelp()
{
  // TODO: Add your command handler code here
}

//------------------------------------------------------------------------------
//Function:	
//------------------------------------------------------------------------------
void CPIDPCApp::HtmlHelp(DWORD_PTR dwData, UINT nCmd)
{
  //TODO: Add your specialized code here and/or call the base class
  CWinAppEx::HtmlHelp(dwData, nCmd);
}

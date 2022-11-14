//==============================================================================
//File name:    "time-stamp.cpp"
//Purpose:      Source File, CPage2 property page
//Version:      1.00
//Copyright:    (c) 2019, Akimov Vladimir  E-mail: decoder@rambler.ru	
//==============================================================================
#include "stdafx.h"
#include "time-stamp.h"

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
//Constructor
//------------------------------------------------------------------------------
CTSTAMP::CTSTAMP()
{ 

}

//------------------------------------------------------------------------------
//destructor
//------------------------------------------------------------------------------
CTSTAMP::~CTSTAMP()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CTSTAMP::GetStamp(CString *pTxt)
{
  CString stamp, txt;
  stamp.Empty();

  //Get time
  GetLocalTime(&wtime);
    
  //---- create step time
  txt.Format("%d:",wtime.wHour);
  stamp = txt;
  txt.Format("%d:",wtime.wMinute);
  stamp += txt;
  txt.Format("%d:",wtime.wSecond);
  stamp += txt;
  txt.Format("%d",wtime.wMilliseconds);
  stamp += txt;

  *pTxt = stamp;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CTSTAMP::GetStampStd(std::string *pStdStr)
{
  //Get time
  GetLocalTime(&wtime);
  std::string stamp;

  //---- create 
  stamp =  std::to_string((long long)wtime.wHour);
  stamp += std::to_string((long long)wtime.wMinute);
  stamp += std::to_string((long long)wtime.wSecond);
  stamp += std::to_string((long long)wtime.wMilliseconds);
  
  *pStdStr = stamp;
}
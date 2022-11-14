//==============================================================================
//File name:   "time-stamp.h"
//Purpose:      Header File
//Version:      1.00
//Copyright:    (c) 2019, Akimov Vladimir  E-mail: decoder@rambler.ru		
//==============================================================================
#include "afxcmn.h"
#include "afxwin.h"
#include <string>

#ifndef _TIME_STAMP_CLASS_H_
#define _TIME_STAMP_CLASS_H_

//------------------------------------------------------------------------------
// CLASS
//------------------------------------------------------------------------------
class CTSTAMP
{ 
  //Construction
  public:
  CTSTAMP();
 ~CTSTAMP();
  
  void GetStamp(CString *pTxt);
  void GetStampStd(std::string *pStdStr);

  SYSTEMTIME wtime;
};

#endif
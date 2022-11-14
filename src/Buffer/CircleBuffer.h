//==============================================================================
//File name:   "CircleBuffer.h"
//Purpose:      Header File
//Version:      1.00
//Copyright:    (c) 2011, Akimov Vladimir  E-mail: decoder@rambler.ru		
//==============================================================================
#include <afxmt.h>	  //MFC: CCriticalSection, CEvent

#ifndef _CIRCLE_BUFFER_H_
#define _CIRCLE_BUFFER_H_

//------------------------------------------------------------------------------
// CUSBbuffer class
//------------------------------------------------------------------------------
class CCircleBuffer : public CObject
{
//Construction
public:
	CCircleBuffer(int nLength = 20, int nExtra = 10);
	unsigned char *pBuffer;
    unsigned int  m_Size;

//Attributes
private:
	
	int m_Length, m_Extra;
    int m_Head, m_Tail; 

	CCriticalSection m_Lock;

public:
	CEvent flag_bufferNotEmpty;
	CEvent flag_sendBlock;

//---- Operations
public:
	void WriteByte(unsigned char Byte);	    //Write 1 byte
	bool ReadByte (unsigned char &Byte);    //Read  1 byte
	
	void WriteBlock(unsigned char *pTxBuf, int size); //Write block
	bool ReadBlock(unsigned char *pRxBuf);            //Read  block

//---- Implementation
public:
	virtual ~CCircleBuffer();
};
#endif

#include "StdAfx.h"
#include "ListenThread.h"
#include "mediaPlayer.h"
#include "cs/editCs.h"

CListenThread::CListenThread(CmediaPlayerApp& obj, CEditCs& EditCs)
:m_theApp(obj),
 m_EditRef(EditCs)
{
}

CListenThread::~CListenThread(void)
{
}

void CListenThread::Main()
{
  char szBuff[MAX_CMD_LEN] = {'\0'};

  while (!m_Terminate)
  {
  }

  TRACE(_T("CListenThread exit\r\n"));
}

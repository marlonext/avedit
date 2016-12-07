#pragma once
#include "cs\lowsys.h"

class CEditCs;
class CmediaPlayerApp;

class CListenThread :
  public CSumThread
{
public:
  CListenThread(CmediaPlayerApp& obj, CEditCs& EditCs);
  virtual ~CListenThread(void);

  virtual void Main();

private:
  CmediaPlayerApp& m_theApp;
  CEditCs& m_EditRef;
};

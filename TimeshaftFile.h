#pragma once

#include "stdafx.h"
#include <fstream>

class CTimeshaftFile;

class CFmtTime
{
  friend class CTimeshaftFile;

public:
  CFmtTime();
  CFmtTime(int64_t uSecs);

  std::string AsString();

  int Hour();
  int nMin();
  int nSec();

private:
  void Init();

private:
  int m_nHour;
  int m_nMin;
  int m_nSec;
  int64_t m_uSec;
};



class CTimeshaftFile
{
public:
  CTimeshaftFile(void);
  virtual ~CTimeshaftFile(void);
  
  void Write(const char* pBuff, int nLen) /*noexcept*/;
  void Read(char* pBuff, int nLen);
  
private:
  std::fstream m_ios;

  static const std::string m_strOut;
};


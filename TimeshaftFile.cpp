#include "StdAfx.h"
#include "TimeshaftFile.h"
#include <string>
#include <assert.h>

const std::string CTimeshaftFile::m_strOut = "time.dat";

CTimeshaftFile::CTimeshaftFile(void)
{
  if (!m_ios.is_open())
  {
    m_ios.open(m_strOut, std::ios::binary | std::ios::out | std::ios::in);
  }

  m_ios.seekp(0);
}


CTimeshaftFile::~CTimeshaftFile(void)
{
  if (m_ios.is_open())
  {
    m_ios.close();
  }
}

void CTimeshaftFile::Write(const char* pBuff, int nLen)
{
  assert(pBuff != nullptr);

  m_ios.write(pBuff, nLen);
}

void CTimeshaftFile::Read(char* pBuff, int nLen)
{
  assert(pBuff != nullptr);

  m_ios.read(pBuff, nLen);
}

//////////////////////////////////////////////////////////////////////////
CFmtTime::CFmtTime()
{
  Init();
}

CFmtTime::CFmtTime(int64_t uSecs)
{
  Init();
  m_uSec = uSecs;
}


void CFmtTime::Init()
{
  m_nHour = 0;
  m_nMin = 0;
  m_nSec = 0;
  m_uSec = 0;
}


// ±∑÷√Î
std::string CFmtTime::AsString()
{
  int nSecs = m_uSec / (1000 * 1000);
  int nHours = 0;
  int nMins = 0;

  do 
  {
    nMins += nSecs / 60;
    nHours += nMins / 60;

    nSecs %= 60;
  } while (nSecs < 60);

  m_nHour = nHours;
  m_nMin = nMins;
  m_nSec = nSecs;

  char szBuff[13] = {0};
  sscanf(szBuff, "%d%d%d", &nHours, &nMins, &nSecs);

  std::string strTemp = szBuff;

  return strTemp;
}

int CFmtTime::Hour()
{
  return m_nHour;
}

int CFmtTime::nMin()
{
  return m_nMin;
}

int CFmtTime::nSec()
{
  return m_nSec;
}



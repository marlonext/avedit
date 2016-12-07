#include "StdAfx.h"
#include "EditStatueFile.h"
#include "SAStatusLog.h"

extern CSAStatusLog g_statusLog;

char STATUESFILE[] = "tasklist.dat";

CEditStatueFile::CEditStatueFile(void)
{
  m_nStatusCount = 0; 
  m_fpStatues = NULL;
  m_bInit = false;
}


CEditStatueFile::~CEditStatueFile(void)
{
  if (m_fpStatues)
  {
    ::fclose(m_fpStatues);
  }

  m_bInit = false;
}

bool CEditStatueFile::Init()
{
  g_statusLog.StatusOut("CEditStatueFile::Init()");
  TCHAR szDir[256];
  GetModuleFileName(AfxGetInstanceHandle(), szDir, sizeof(szDir));

  CString strDir = szDir;
  int nFind = strDir.ReverseFind('\\');
  CString strLeft = strDir.Left(nFind);
  strLeft += "\\";
  strLeft += STATUESFILE;

  errno_t err = ::fopen_s(&m_fpStatues, strLeft, "rb+");
  if (err != 0)
  {
    err = ::fopen_s(&m_fpStatues, strLeft, "wb+");
    if (err != 0)
    {
      m_bInit = false;
      return false;
    }
    TRACE(_T("open failed\r\n"));
  }

  m_bInit = true;
  return true;
}

BOOL CEditStatueFile::Read(EDITSTATUSLIST& list)
{
  m_cs.Lock();

  ::rewind(m_fpStatues);
  int nRead = ::fread(&m_nStatusCount, sizeof(m_nStatusCount), 1, m_fpStatues);
  if (ferror(m_fpStatues))
  {
    m_cs.Unlock();
    return FALSE;
  }

  int nValidStatusCount = 0;
  EDITSTATUS temp;
  while (true)
  {
    ::fread(&temp, sizeof(temp), 1, m_fpStatues);
    if (ferror(m_fpStatues))
    {
      return FALSE;
    }

    if (1 == temp.m_bUseFlag)
    {
      list.push_back(temp);

      if (++nValidStatusCount >= m_nStatusCount)
      {
        break;
      }
    }

  }

  m_cs.Unlock();

  return TRUE;
}


BOOL CEditStatueFile::Write(const EDITSTATUS& statues)
{
  int nWrite = 0;
  BOOL bFindWritePos = FALSE;
  int nOffSet = 0;
  int nValidCount = 0;
  m_cs.Lock();

  ::rewind(m_fpStatues);

  int nRead = ::fread(&m_nStatusCount, sizeof(m_nStatusCount), 1, m_fpStatues);

  EDITSTATUS temp;
  while (true)
  {
    ::fread(&temp, sizeof(temp), 1, m_fpStatues);
    //update
    if (1 == temp.m_bUseFlag && 0 == _tcscmp((char*)(statues.m_szSourceFileName), temp.m_szSourceFileName))
    {
      ::fseek(m_fpStatues, -sizeof(EDITSTATUS), SEEK_CUR);
      ::fwrite(&statues, sizeof(EDITSTATUS), 1, m_fpStatues);
      ::fflush(m_fpStatues);
      return TRUE;
    }

    if (0 == temp.m_bUseFlag)
    {
      ::fseek(m_fpStatues, -sizeof(EDITSTATUS), SEEK_CUR);
      nWrite = ::fwrite(&statues, sizeof(statues), 1, m_fpStatues);

      ++m_nStatusCount;
      rewind(m_fpStatues);
      nWrite = ::fwrite(&m_nStatusCount, sizeof(m_nStatusCount), 1, m_fpStatues);
      fflush(m_fpStatues);
      m_cs.Unlock();
      return TRUE;      
    }

    if (++nValidCount >= m_nStatusCount)
    {
      break;
    }

    nOffSet += sizeof(statues);
  }

  if (0 == m_nStatusCount)
  {
    rewind(m_fpStatues);
    ++m_nStatusCount;
    nWrite = ::fwrite(&m_nStatusCount, sizeof(m_nStatusCount), 1, m_fpStatues);
    nWrite = ::fwrite(&statues, sizeof(statues), 1, m_fpStatues);
    fflush(m_fpStatues);
    return TRUE;
  }
  else
  {
    ::fseek(m_fpStatues, 0, SEEK_CUR);
    nWrite = ::fwrite(&statues, sizeof(statues), 1, m_fpStatues);
    ::fflush(m_fpStatues);
    if (ferror(m_fpStatues))
    {
      m_cs.Unlock();
      return FALSE;
    }
  }

  ++m_nStatusCount;
  rewind(m_fpStatues);
  nWrite = ::fwrite(&m_nStatusCount, sizeof(m_nStatusCount), 1, m_fpStatues);
  ::fflush(m_fpStatues);

  m_cs.Unlock();
  return TRUE;
}

BOOL CEditStatueFile::Update(const EDITSTATUS& status)
{
  m_cs.Lock();
  ::rewind(m_fpStatues);

  int nRead = ::fread(&m_nStatusCount, sizeof(m_nStatusCount), 1, m_fpStatues);
  if (ferror(m_fpStatues))
  {
    m_cs.Unlock();
    return FALSE;
  }

  EDITSTATUS temp;

  for (int i = 0; i < m_nStatusCount; ++i)
  {
    ::fread(&temp, 1, sizeof(temp), m_fpStatues);
    if (temp.m_nTaskId == status.m_nTaskId)
    {
      ::fseek(m_fpStatues, -sizeof(EDITSTATUS), SEEK_CUR);
      ::fwrite(&status, 1, sizeof(EDITSTATUS), m_fpStatues);
      ::fflush(m_fpStatues);

      m_cs.Unlock();
      return TRUE;
    }
  }

  m_cs.Unlock();
  return FALSE;
}

BOOL CEditStatueFile::Delete(UInt16b nTaskId)
{
  m_cs.Lock();

  ::rewind(m_fpStatues);

  int nRead = ::fread(&m_nStatusCount, 1, sizeof(m_nStatusCount), m_fpStatues);
  if (ferror(m_fpStatues))
  {
    m_cs.Unlock();
    return FALSE;
  }

  BOOL bDel = FALSE;
  int nValidStatusCount = 0;
  EDITSTATUS temp;
  int nSize = sizeof(EDITSTATUS);
  while (true)
  {
    ::fread(&temp, sizeof(temp), 1, m_fpStatues);
    if (nTaskId == temp.m_nTaskId)
    {
      ::fseek(m_fpStatues, -sizeof(temp.m_bUseFlag), SEEK_CUR);

      UInt16b b = 0;
      int nWrite = ::fwrite(&b, sizeof(b), 1, m_fpStatues);

      ::rewind(m_fpStatues);
      --m_nStatusCount;
      nWrite = ::fwrite(&m_nStatusCount, sizeof(m_nStatusCount), 1, m_fpStatues);
      fflush(m_fpStatues);

      m_cs.Unlock();
      bDel = TRUE;

      break;
    }

    if (temp.m_bUseFlag && ++nValidStatusCount >= m_nStatusCount)
    {
      bDel = FALSE;
      break;
    }
  }

  if (bDel)
  {
    return TRUE;
  }
  
  return FALSE;
}

BOOL CEditStatueFile::IsEmpty()
{
  if (!m_fpStatues)
  {
    return FALSE;
  }

  ::fread(&m_nStatusCount, 1, sizeof(m_nStatusCount), m_fpStatues);

  return m_nStatusCount == 0 ? TRUE : FALSE;
}

BOOL CEditStatueFile::Clear()
{
  m_cs.Lock();
  ::rewind(m_fpStatues);

  int nCount;
  ::fread(&nCount, sizeof(nCount), 1, m_fpStatues);

  int nValidCount = 0;
  while (true)
  {
    EDITSTATUS temp;
    ::fread(&temp, sizeof(temp), 1, m_fpStatues);

    if (1 == temp.m_bUseFlag)
    {
      ::fseek(m_fpStatues, -sizeof(temp.m_bUseFlag), SEEK_CUR);

      const UInt16b b = 0;
      ::fwrite(&b, sizeof(b), 1, m_fpStatues);
      ::fflush(m_fpStatues);

      --nCount;
    }

    if (0 == nCount)
    {
      rewind(m_fpStatues);
      ::fwrite(&nCount, sizeof(nCount), 1, m_fpStatues);
      ::fflush(m_fpStatues);

      m_cs.Unlock();
      return TRUE;
    }
  }

  m_cs.Unlock();
  return FALSE;
}



//////////////////////////////////////////////////////////////////////////
char const* const TIMERECORDFILE = "timerecord.dat";


CTimeRecord::CTimeRecord()
{
  m_nCount = 0;

  m_hFile = INVALID_HANDLE_VALUE;
}

CTimeRecord::~CTimeRecord()
{
  CloseHandle(m_hFile);
}

bool CTimeRecord::Init()
{
  TCHAR szDir[256];
  GetModuleFileName(AfxGetInstanceHandle(), szDir, sizeof(szDir));

  CString strDir = szDir;
  int nFind = strDir.ReverseFind('\\');
  CString strLeft = strDir.Left(nFind);
  strLeft += "\\";
  strLeft += TIMERECORDFILE;

//   errno_t err = ::fopen_s(&m_fpTime, strLeft, "rb+");
//   if (err != 0)
//   {
//     TRACE(_T("open failed\r\n"));
//   }

  m_hFile = CreateFile(strLeft, GENERIC_READ|GENERIC_WRITE, 
                       FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, 
                       /*CREATE_NEW|*/OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if (INVALID_HANDLE_VALUE == m_hFile)
  {
    m_hFile = CreateFile(strLeft, GENERIC_READ|GENERIC_WRITE, 
      FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, 
      CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == m_hFile)
    {
      TRACE(_T("open failed\r\n"));
      return false;
    }
  }

  return true;
}

BOOL CTimeRecord::Read(EDIT_TASK_LIST& taskList)
{
  SYSTEMTIME now;
  int nRecordLen = 0;
  int nOffSetLen = 0;

  m_cs.Lock();

  SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN);

  DWORD dwRead;
  BOOL bRet = ReadFile(m_hFile, &m_nCount, sizeof(m_nCount), &dwRead, NULL);
  if (FALSE == bRet)
  {
    m_cs.Unlock();
    return FALSE;
  }

  EDIT_TASK_RECORD* pRecord = nullptr;
  int nValidCount = 0;
  while (true)
  {
    EDIT_TASK_RECORD record;
    ZeroMemory(&record, sizeof(record));
    nRecordLen = 0;
    nOffSetLen = 0;

    ReadFile(m_hFile, &nRecordLen, sizeof(nRecordLen), &dwRead, NULL);
    if (0 == dwRead)
    {
      break;
    }

    int nTaskId;
    ReadFile(m_hFile, &nTaskId, sizeof(nTaskId), &dwRead, NULL);
    record.m_nID = nTaskId;

    nOffSetLen += sizeof(nTaskId);

    ReadFile(m_hFile, &record.m_editOpts, sizeof(CS_EDIT_OPTS), &dwRead, NULL);
    nOffSetLen += sizeof(CS_EDIT_OPTS);

    int nCutTimeCount = 0;
    ReadFile(m_hFile, &nCutTimeCount, sizeof(nCutTimeCount), &dwRead, NULL);
    record.m_nCutTimeCount = nCutTimeCount;
    nOffSetLen += sizeof(nCutTimeCount);

    SetFilePointer(m_hFile, nCutTimeCount*sizeof(CS_CUT_TIME), NULL, FILE_CURRENT);
    nOffSetLen += nCutTimeCount*sizeof(CS_CUT_TIME);

    ReadFile(m_hFile, &record.m_bFlag, sizeof(record.m_bFlag), &dwRead, NULL);
    nOffSetLen += sizeof(BYTE);

#if 1
    SYSTEMTIME now;
    GetLocalTime(&now);

    CString strFmt;
    strFmt.Format("recordLen|taskid|CS_EDIT_OPTS(size)|cuttime_number|CS_CUT_TIME(size)|useflag\n\
      %-6d|%-6d|%-6d|%-6d|%-6d|%-6d",
      nRecordLen, nTaskId, sizeof(CS_EDIT_OPTS), nCutTimeCount, nCutTimeCount*sizeof(CS_CUT_TIME), record.m_bFlag);
    g_statusLog.StatusOut(strFmt);
#endif

    if (1 == record.m_bFlag)
    {
      pRecord = new EDIT_TASK_RECORD;

      pRecord->m_nID = record.m_nID;
      memcpy(&pRecord->m_editOpts, &record.m_editOpts, sizeof(CS_EDIT_OPTS));
      pRecord->m_nCutTimeCount = record.m_nCutTimeCount;

      SetFilePointer(m_hFile, -(sizeof(BYTE) + nCutTimeCount*sizeof(CS_CUT_TIME)), NULL, FILE_CURRENT);
      CS_CUT_TIME* pCutTime = new CS_CUT_TIME[nCutTimeCount];
      for (int j = 0; j < nCutTimeCount; ++j)
      {
        ReadFile(m_hFile, &pCutTime[j], sizeof(CS_CUT_TIME), &dwRead, NULL);
      }
      pRecord->m_pCutTime = pCutTime;

      ++nValidCount;
      taskList.push_back(pRecord);

      SetFilePointer(m_hFile, sizeof(BYTE), NULL, FILE_CURRENT);
      SetFilePointer(m_hFile, nRecordLen-nOffSetLen, NULL, FILE_CURRENT);      
    }
  }

  m_cs.Unlock();
  return TRUE;
}

//
//totalLen|number|taskid|CS_EDIT_OPTS|cuttime_number|CS_CUT_TIME|flag

BOOL CTimeRecord::Write(const EDIT_TASK_RECORD& editTaskRecord)
{
  BYTE bFlag = 0;
  int nOffSet = 0, nOffSetTemp = 0, nRecordLen = 0;
  DWORD dwRead, dwWrite, dwFilePoint;
  BOOL bRet = FALSE;
  BOOL bFileEnd = FALSE;
  BOOL bSameID = FALSE;
  int nValidCount = 0;
  int nTaskId;

  int nWriteTaskLen = sizeof(editTaskRecord.m_nID) + sizeof(editTaskRecord.m_editOpts)
    + sizeof(editTaskRecord.m_nCutTimeCount) + editTaskRecord.m_nCutTimeCount*sizeof(CS_CUT_TIME)
    +sizeof(editTaskRecord.m_bFlag);

  m_cs.Lock();

  SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN);
  bRet = ReadFile(m_hFile, &m_nCount, sizeof(m_nCount), &dwRead, NULL);
  if (FALSE == bRet)
  {
    return FALSE;
  }

  SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN);
  WriteFile(m_hFile, &m_nCount, sizeof(m_nCount), &dwWrite, NULL);

  while (true)
  {
    //totalLen
    bRet = ReadFile(m_hFile, &nRecordLen, sizeof(nRecordLen), &dwRead, NULL);
    if (0 == dwRead)
    {
      bFileEnd = TRUE;
      break;
    }
    nOffSet += sizeof(nRecordLen);

    bRet = ReadFile(m_hFile, &nTaskId, sizeof(nTaskId), &dwRead, NULL);
    if (nTaskId < 1 || nTaskId > 20)
    {
      int n = 0;
    }

    nOffSet += sizeof(nTaskId);

    SetFilePointer(m_hFile, sizeof(CS_EDIT_OPTS), NULL, FILE_CURRENT);
    nOffSet += sizeof(CS_EDIT_OPTS);

    int nCutTimeCount = 0;
    bRet = ReadFile(m_hFile, &nCutTimeCount, sizeof(nCutTimeCount), &dwRead, NULL);
    if (FALSE == bRet)
    {
      m_cs.Unlock();
      return FALSE;
    }
    nOffSet += sizeof(nCutTimeCount);

    SetFilePointer(m_hFile, nCutTimeCount * sizeof(CS_CUT_TIME), NULL, FILE_CURRENT);
    nOffSet += nCutTimeCount*sizeof(CS_CUT_TIME);
    
    BYTE bFlag;
    bRet = ReadFile(m_hFile, &bFlag, sizeof(bFlag), &dwRead, NULL);
    if (FALSE == bRet)
    {
      m_cs.Unlock();
      return FALSE;
    }

    nOffSet += sizeof(bFlag);
    nOffSetTemp = nOffSet;

    if (bFlag)
    {
      if (nTaskId == editTaskRecord.m_nID)
      {
        bSameID = TRUE;

        if (nWriteTaskLen <= nOffSet)
        {
          SetFilePointer(m_hFile, -nOffSet, NULL, FILE_CURRENT);
          break;
        }
        else
        {
          SetFilePointer(m_hFile, -sizeof(BYTE), NULL, FILE_CURRENT);

          bFlag = 0;
          WriteFile(m_hFile, &bFlag, sizeof(bFlag), &dwWrite, NULL);
          --m_nCount;
        }
      }
      else
      {
        bSameID = FALSE;
      }
    }
    else
    {
      if (nWriteTaskLen <= nOffSet)
      {
        SetFilePointer(m_hFile, -nOffSet, NULL, FILE_CURRENT);
        break;
      }
    }

    SetFilePointer(m_hFile, (nRecordLen+sizeof(int))-nOffSet, 0, FILE_CURRENT);
    nOffSet = 0;
  }

  if (bFileEnd && 0 == bFlag)
  {
    SetFilePointer(m_hFile, -nOffSetTemp, NULL, FILE_CURRENT);
  }

  if (nWriteTaskLen >= nOffSetTemp)
  {
    WriteFile(m_hFile, &nWriteTaskLen, sizeof(nWriteTaskLen), &dwWrite, NULL);
  }
  else
  {
    WriteFile(m_hFile, &nOffSetTemp, sizeof(nOffSetTemp), &dwWrite, NULL);
  }

  WriteFile(m_hFile, &editTaskRecord.m_nID, sizeof(editTaskRecord.m_nID), &dwWrite, NULL);
  WriteFile(m_hFile, &editTaskRecord.m_editOpts, sizeof(editTaskRecord.m_editOpts), &dwWrite, NULL);

  int nCutTimeCount = editTaskRecord.m_nCutTimeCount;
  bRet = WriteFile(m_hFile, &nCutTimeCount, sizeof(nCutTimeCount), &dwWrite, NULL);
  for (int j = 0; j < nCutTimeCount; ++j)
  {
    bRet = WriteFile(m_hFile, &editTaskRecord.m_pCutTime[j], sizeof(CS_CUT_TIME), &dwWrite, NULL);
    FlushFileBuffers(m_hFile);
  }

  bFlag = 1;
  bRet = WriteFile(m_hFile, &bFlag, sizeof(bFlag), &dwWrite, NULL);
  FlushFileBuffers(m_hFile);

  SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN);

  if (!bSameID)
  {
    ++m_nCount;
  }
  bRet = WriteFile(m_hFile, &m_nCount, sizeof(m_nCount), &dwWrite, NULL);
  FlushFileBuffers(m_hFile);

  m_cs.Unlock();

  return TRUE;
}


//number|taskid|CS_EDIT_OPTS|cuttime_number|CS_CUT_TIME|flag
BOOL CTimeRecord::Delete(int nTaskId)
{
  m_cs.Lock();

  SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN);

  int nItemCount = 0;
  DWORD dwRead = 0;
  DWORD dwWrite = 0;
  BOOL bRet = FALSE;
  BOOL bFindTaskID = FALSE;
  int nOffSet = 0;
  BYTE bFlag = 0;
  int nRecordLen = 0;

  bRet = ReadFile(m_hFile, &nItemCount, sizeof(nItemCount), &dwRead, NULL);
  if (FALSE == bRet)
  {
    m_cs.Unlock();
    return FALSE;
  }

  while (true)
  {
    ReadFile(m_hFile, &nRecordLen, sizeof(nRecordLen), &dwRead, NULL);
    if (0 == dwRead)
    {
      return FALSE;
    }

    int nDelTaskId;
    ReadFile(m_hFile, &nDelTaskId, sizeof(nDelTaskId), &dwRead, NULL);
    nOffSet += sizeof(nDelTaskId);

    SetFilePointer(m_hFile, sizeof(CS_EDIT_OPTS), NULL, FILE_CURRENT);
    nOffSet += sizeof(CS_EDIT_OPTS);

    int nCutTimeCount = 0;
    ReadFile(m_hFile, &nCutTimeCount, sizeof(nCutTimeCount), &dwRead, NULL);
    nOffSet += sizeof(nCutTimeCount);

    SetFilePointer(m_hFile, nCutTimeCount*sizeof(CS_CUT_TIME), NULL, FILE_CURRENT);
    nOffSet += nCutTimeCount*sizeof(CS_CUT_TIME);

    ReadFile(m_hFile, &bFlag, sizeof(bFlag), &dwRead, NULL);
    nOffSet += sizeof(bFlag);

    if (nDelTaskId == nTaskId && TRUE == bFlag)
    {
      bFindTaskID = TRUE;
      break;
    }

    SetFilePointer(m_hFile, nRecordLen-nOffSet, NULL, FILE_CURRENT);
  }

  if (bFindTaskID/* &&  (1 == bFlag)*/)
  {
    SetFilePointer(m_hFile, -sizeof(BYTE), NULL, FILE_CURRENT);

    bFlag = 0;
    bRet = WriteFile(m_hFile, &bFlag, sizeof(bFlag), &dwWrite, NULL);

    --nItemCount;
    SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN);
    WriteFile(m_hFile, &nItemCount, sizeof(nItemCount), &dwWrite, NULL);    
    FlushFileBuffers(m_hFile);

    m_cs.Unlock();
    return TRUE;
  }

  m_cs.Unlock();
  return FALSE;
}

//number|taskid|CS_EDIT_OPTS|cuttime_number|CS_CUT_TIME|flag
BOOL CTimeRecord::Clear()
{
  int nCount = 0;
  int nValidCount = 0;
  DWORD dwRead, dwWrite;

  m_cs.Lock();

  ReadFile(m_hFile, &nCount, sizeof(nCount), &dwRead, NULL);

  while (true)
  {
    SetFilePointer(m_hFile, sizeof(int) + sizeof(CS_EDIT_OPTS), NULL, FILE_CURRENT);

    int nCutTimeNumber;
    ReadFile(m_hFile, &nCutTimeNumber, sizeof(nCutTimeNumber), &dwRead, NULL);
    SetFilePointer(m_hFile, nCutTimeNumber * sizeof(CS_CUT_TIME), NULL, FILE_CURRENT);
    
    BYTE bFlag;
    ReadFile(m_hFile, &bFlag, sizeof(bFlag), &dwRead, NULL);
    if (1 == bFlag)
    {
      SetFilePointer(m_hFile, -sizeof(bFlag), NULL, FILE_CURRENT);

      BYTE bNoUse = 0;
      WriteFile(m_hFile, &bNoUse, sizeof(bNoUse), &dwWrite, NULL);
      FlushFileBuffers(m_hFile);

      --nCount;
    }

    if (0 == nCount)
    {
      SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN);
      WriteFile(m_hFile, &nCount, sizeof(nCount), &dwWrite, NULL);
      FlushFileBuffers(m_hFile);

      m_cs.Unlock();
      return TRUE;
    }
  }

  m_cs.Unlock();
  return FALSE;
}

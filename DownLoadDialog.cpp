// DownLoadDialog.cpp : implementation file
//

#include "stdafx.h"
#include "mediaPlayer.h"
#include "DownLoadDialog.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "FilePreView.h"
#include "FilePrevDialog.h"
#include "TaskListDialog.h"
#include <afx.h>
#include <afxinet.h>
#include <direct.h>
#include "URL.h"

// CDownLoadDialog dialog
extern CMainFrame* g_pMainFrame;
IMPLEMENT_DYNAMIC(CDownLoadDialog, CDialogEx)

  CDownLoadDialog::CDownLoadDialog(CWnd* pParent /*=NULL*/)
  : CDialogEx(CDownLoadDialog::IDD, pParent)
{
  m_nFileLen = 0;
  m_nDownLen = 0;
  m_strDir = _T("");
  m_StrMp4Name = _T("");
  m_strAlias  =_T("");
  m_nFileLen = m_nDownLen = 0;
  m_hHttpThread = INVALID_HANDLE_VALUE;
  m_bDownLoadSucess = FALSE;
}

CDownLoadDialog::~CDownLoadDialog()
{
  if (m_hHttpThread)
  {
    CloseHandle(m_hHttpThread);
    m_hHttpThread = INVALID_HANDLE_VALUE;
  }
}

void CDownLoadDialog::DoDataExchange(CDataExchange* pDX)
{
  CDialogEx::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_PROGRESS_DOWN, m_Progress);
}


BEGIN_MESSAGE_MAP(CDownLoadDialog, CDialogEx)
  ON_MESSAGE(WM_START_DOWNLOAD, OnStartDownLoad) 
  ON_MESSAGE(WM_DOWNLOADSUCESS, OnDownLoadSuccess)
  ON_MESSAGE(WM_DOWNLOADFAILURE, OnDownLoadFailure)
  ON_WM_TIMER()
END_MESSAGE_MAP()


// CDownLoadDialog message handlers

// void CDownLoadDialog::SetDownLoadSource(const CString& strPath)
// {      
//   //http download
//   m_strUrl = strPath;
// }



BOOL CDownLoadDialog::OnInitDialog()
{
  CDialogEx::OnInitDialog();

  // TODO:  Add extra initialization here
  m_Progress.SetRange(0, m_nMaxPos);
  m_Progress.SetPos(0);
  m_Progress.SetStep(10);

  CString strDir = _T("D:\\avEdit");
  CreateDirectory(strDir, NULL);
  strDir += _T("\\mp4");
  CreateDirectory(strDir, NULL);
  SetCurrentDirectory(strDir);

  const int nNameStart = CURL::GetURL().ReverseFind(_T('//'));
  m_StrMp4Name = CURL::GetURL().Right(CURL::GetURL().GetLength()-nNameStart-1);
  m_strDir = strDir;

  m_hHttpThread = (HANDLE)_beginthreadex(nullptr, 0, HttpDownLoadThread, 
                this, CREATE_SUSPENDED, nullptr);


  PostMessage(WM_START_DOWNLOAD);

//   if (FindFiles(strDir))
//   {
//     //�����ص��ļ������������ļ�
//     PostMessage(WM_DOWNLOADSUCESS);
//   }
//   else
//   {
//     PostMessage(WM_START_DOWNLOAD);
//   }
  

  return TRUE;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CDownLoadDialog::OnStartDownLoad(WPARAM wParam, LPARAM lParam)
{
  ::ResumeThread(m_hHttpThread);

  return 0;
}


void CDownLoadDialog::OnTimer(UINT_PTR nIDEvent)
{
  // TODO: Add your message handler code here and/or call default
//   if (WM_START_DOWNLOAD == nIDEvent)
//   {
//     PostMessage(WM_START_DOWNLOAD);
//     KillTimer(WM_START_DOWNLOAD);
//   }

  CDialogEx::OnTimer(nIDEvent);
}

//��ֹ����ҳ�濨��
unsigned int __stdcall CDownLoadDialog::HttpDownLoadThread(void* pParam)
{
  CDownLoadDialog* pThis = (CDownLoadDialog*)pParam;

  pThis->Main();

  return 0;
}

void CDownLoadDialog::Main()
{
  CFile file/*(m_StrMp4Name, CFile::modeCreate | CFile::modeWrite)*/;

  //�����ļ�������Ϊ���ļ���
  bool ret = false;
  CInternetSession Sess("lpload");
  Sess.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT     , 2000); //2������ӳ�ʱ
  Sess.SetOption(INTERNET_OPTION_SEND_TIMEOUT        , 2000); //2��ķ��ͳ�ʱ
  Sess.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT     , 2000); //2��Ľ��ճ�ʱ
  Sess.SetOption(INTERNET_OPTION_DATA_SEND_TIMEOUT   , 2000); //2��ķ��ͳ�ʱ
  Sess.SetOption(INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, 2000); //2��Ľ��ճ�ʱ
  DWORD dwFlag = INTERNET_FLAG_TRANSFER_BINARY|INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_RELOAD ;

  CHttpFile* pHttpFile   = NULL;
  char      *pBuf    = NULL;
  int        nBufLen = 0   ;
  do {
    try{
      pHttpFile = (CHttpFile*)Sess.OpenURL(CURL::GetURL(), 1, dwFlag);
      if (nullptr == pHttpFile)
      {
        break;
      }

      DWORD dwStatusCode;
      pHttpFile->QueryInfoStatusCode(dwStatusCode);
      if (dwStatusCode == HTTP_STATUS_OK) 
      {
        //��ѯ�ļ�����
        DWORD nLen=0;
        pHttpFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, nLen);
        //CString strFilename = GetFileName(url,TRUE);
        m_nFileLen = nBufLen=nLen;
        if (nLen <= 0) break;//

        //����������ݻ���
        //         pBuf = (char*)malloc(nLen+8);
        pBuf = (char*)malloc(RECVPACK_SIZE);
        if (!pBuf)
        {
          ret = false;
          break;
        }

        ZeroMemory(pBuf, RECVPACK_SIZE);

        BOOL bRet = file.Open(m_StrMp4Name, CFile::modeCreate | CFile::modeWrite);
        if (FALSE == bRet)
        {
          return;
        }

        //         char *p=pBuf;
        while (nLen>0) 
        {
          //ÿ������8K
          //           int n = cFile->Read(pBuf, (nLen<RECVPACK_SIZE) ? nLen:RECVPACK_SIZE);
          int n = pHttpFile->Read(pBuf, RECVPACK_SIZE);
          m_nDownLen += n;

          file.Write(pBuf, n);

          int nPos = (float)m_nDownLen * m_nMaxPos / (float)m_nFileLen ;
          m_Progress.SetPos(nPos);

          //��������˳�ѭ��
          if (n <= 0) break;//
          //���ջ������
          //           p+= n ;

          //ʣ�೤�ȵݼ�
          nLen -= n ;
        }

        file.Close();
        //���δ�������ж��˳�
        if (nLen != 0) break;

        m_Progress.SetPos(200);
        //���ճɹ����浽�ļ�

        ret = true;
      }
    } 
    catch(...) 
    {
      break;//
    }
  } while(0);

  //�ͷŻ���
  if (pBuf) 
  {
    free(pBuf);
    pBuf=NULL;
    nBufLen = 0 ;
  }

  //�ر���������
  if (pHttpFile) 
  {
    pHttpFile->Close();
    Sess.Close();
    delete pHttpFile;
  }

  if (ret)
  {
    PostMessage(WM_DOWNLOADSUCESS);
  }
  else
  {
    PostMessage(WM_DOWNLOADFAILURE);
  }
}

LRESULT CDownLoadDialog::OnDownLoadSuccess(WPARAM wPara, LPARAM lParam)
{
  if (!g_pMainFrame)
  {
    return 0;
  }
  m_bDownLoadSucess = TRUE;

  CSplitterWnd& split = g_pMainFrame->GetSplitterWnd2();
  CFilePreView* pFilePrevView = (CFilePreView*)(split.GetPane(1, 0));
  if (pFilePrevView)
  {
    CEditTaskListCtrl& listCtrl = pFilePrevView->GetTaskListDlg()->GetListCtrl();
    if (listCtrl.GetSafeHwnd())
    {
      m_strDir += "\\";
      listCtrl.SetTaskDir(m_strDir += CURL::GetTaskName());
      ::PostMessage(listCtrl.GetSafeHwnd(), WM_ADDMP4, NULL, NULL);
    }
  }

  CDialog::OnOK();

  return 0;
}

LRESULT CDownLoadDialog::OnDownLoadFailure(WPARAM wPara, LPARAM lParam)
{
  m_bDownLoadSucess = FALSE;
  DeleteFile(m_strDir + "\\" + CURL::GetTaskName());

  AfxMessageBox(_T("����ʧ��"));
  CDialog::OnOK();

  return 0;
}

BOOL CDownLoadDialog::FindFiles(CString &strPath)
{
  WIN32_FIND_DATA findData;

  CString strDest = strPath + "\\" + m_StrMp4Name;
  CString strTemp = strPath;
  strTemp.Format(_T("%s\\*.*"), strPath);//����ָ��Ŀ¼�µ�ֱ�ӵ������ļ���Ŀ¼

  HANDLE hFile = FindFirstFile(strTemp, &findData);
  while (hFile != INVALID_HANDLE_VALUE)
  {
    if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)//�����Ŀ¼
    {
      if(findData.cFileName[0] != _T('.'))//�ų�.��..�ļ���
      {

        strTemp =  strPath+ "\\" + findData.cFileName;//�������·��
        FindFiles(strTemp);         //�ݹ���ҵ�ǰĿ¼���ļ�
      }
    }
    else
    {
      strTemp =  strPath+ "\\" + findData.cFileName;
      if (strDest == strTemp)
      {
        return TRUE;
      }
    }

    if(!FindNextFile(hFile, &findData))
    {
      break;
    }
  }

  return FALSE;
}



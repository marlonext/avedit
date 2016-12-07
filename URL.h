#pragma once


class CURL
{
public:
  CURL(const CString& strURL);
  ~CURL(void);

  static const CString& GetIp() {return m_strIP;}
  static const CString& GetTaskName() {return m_strFileName;}
  static const CString& GetAlias() {return m_strAlias;}
  static const CString& GetURL() {return m_strURL;}

protected:
  void ExtraIP(const CString& strSource);
  void ExtraFileName(const CString& strSource);
  void ExtraAlias(const CString& strSource);
  void ExtraURL(const CString& strSource);

private:
  static CString m_strURL;
  static CString m_strIP;
  static CString m_strFileName;
  static CString m_strAlias;
};


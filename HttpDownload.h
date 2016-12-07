#ifndef HTTP_DOWNLOAD_H
#define HTTP_DOWNLOAD_H

#include <cstdio>
#include <string>
#include <winsock2.h>

class HttpDownload {
public:
	HttpDownload(const char* hostAddr, const int port, 
				 const char* getPath, const char* saveFileName);
	~HttpDownload();
	bool start();
	void cancel();
	void getPos(ULONGLONG& totalSize, ULONGLONG& downloadSize);
protected:
	bool initSocket();						//��ʼ��Socket
	bool sendRequest();						//��������ͷ
	bool receiveData();						//��������
	bool closeTransfer();					//�رմ���
private:
	std::string m_hostAddr;					//Ŀ������IP
	int m_port;								//HTTP�˿ں�
	std::string m_getPath;					//Ŀ���ļ����·��
	std::string m_saveFileName;				//�����ļ�·��
	SOCKET m_sock;							//Socket
	FILE* m_fp;								//�����ļ�ָ��
	ULONGLONG m_fileTotalSize;				//Ŀ���ļ��ܴ�С
	ULONGLONG m_receivedDataSize;			//�ѽ������ݴ�С
	bool m_cancelFlag;						//ȡ�����ر��
};

#endif	//HTTP_DOWNLOAD_H

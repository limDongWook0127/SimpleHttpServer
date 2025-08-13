#pragma once
#include <afxsock.h>
class CClientSocket;
class CListenSocket :
    public CAsyncSocket
{
public:
    CListenSocket();
    virtual ~CListenSocket();

    CPtrList m_ptrClientSocketList; //포인터를 담는 연결리스트 클라이언트 소켓들을 일괄적으로 관리하기 위해 사용함
    virtual void OnAccept(int nErrorCode);
    void CloseClientSokcet(CSocket* pClient);
    void SendChatDataAll(TCHAR* pszMessage);
    void SendBinaryToAll(const void* buf, int len, CClientSocket* pExcept);
   
};


#pragma once
#include <afxsock.h>
#include "CListenSocket.h"
#include "SharedProtocol.h"
class CClientSocket :
    public CSocket
{
public:
    CClientSocket();
    virtual ~CClientSocket();

    void SetListenSocket(CListenSocket* pSocket);
   
    virtual void OnClose(int nErrorCode);
    virtual void OnReceive(int nErrorCode);
    int SendAll(const void* buf, int len);
    int RecvAll(void* buf, int len);
    void SetNickName(BYTE* buffer, int nLen);

    bool IsFileHeader(BYTE* buffer, int nLen);

    void HandleChatMessage(BYTE* buffer, int nLen);

    void HandleFileTransfer(BYTE* buffer, int nLen);

private:
    CListenSocket* m_pListenSocket;
    CString m_NickName;
    bool m_bNickNameReceived = false;
};


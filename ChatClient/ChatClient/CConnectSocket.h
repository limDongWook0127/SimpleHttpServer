#pragma once
#include <afxsock.h>
#include "SharedProtocol.h"
class CConnectSocket :
    public CSocket
{
public:
    virtual void OnClose(int nErrorCode);
    virtual void OnReceive(int nErrorCode);
    int  SendAll(const void* buf, int len);
    int  RecvAll(void* buf, int len);   
    void SendFile(LPCTSTR lpszFilePath);
    bool IsFileHeader(BYTE* buffer, int nLen);
    void HandleReceivedFile(BYTE* buffer, int nLen);
};


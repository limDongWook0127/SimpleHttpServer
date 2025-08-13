#include "pch.h"
#include "CConnectSocket.h"
#include "ChatClientDlg.h"
#include <vector>
#include <algorithm>    // std::min

int CConnectSocket::SendAll(const void* buf, int len)
{
    const char* p = static_cast<const char*>(buf);
    int total = 0;
    while (total < len)
    {
        int n = Send(p + total, len - total); //send(const char* p, int len) p부터 len만큼 보내라!
        if (n == SOCKET_ERROR) return SOCKET_ERROR;
        total += n;
    }
    return total;
}

int CConnectSocket::RecvAll(void* buf, int len)
{
    char* p = static_cast<char*>(buf);
    int total = 0;
    while (total < len)
    {
        int n = Receive(p + total, len - total);
        if (n <= 0) return n; // 0: 끊김, -1: 오류
        total += n;
    }
    return total;
}

void CConnectSocket::OnClose(int nErrorCode)
{
    ShutDown();
    Close();
    CSocket::OnClose(nErrorCode);

    AfxMessageBox(L"ERROR: Disconnected from server");
    ::PostQuitMessage(0);
}

void CConnectSocket::OnReceive(int nErrorCode)
{
    
    BYTE buffer[4096] = { 0 };
    int nLen = Receive(buffer, sizeof(buffer));
    if (nLen <= 0) 
    { 
        CSocket::OnReceive(nErrorCode); 
        return; 
    }

    CChatClientDlg* pMain = (CChatClientDlg*)AfxGetMainWnd();
    if (!pMain)
    {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(pMain->m_queueMutex);
        pMain->m_recvQueue.push(std::vector<BYTE>(buffer, buffer + nLen));
    }
    pMain->m_cv.notify_one();

    CSocket::OnReceive(nErrorCode);
}



bool CConnectSocket::IsFileHeader(BYTE* buffer, int nLen)
{
    if (nLen < (int)sizeof(uint32_t)) return false;  // type조차 없음
    uint32_t type = *(uint32_t*)buffer;
    return (type == 1);
}



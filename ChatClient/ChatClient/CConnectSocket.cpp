#include "pch.h"
#include "CConnectSocket.h"
#include "ChatClientDlg.h"
#include <vector>
#include <algorithm>    // std::min

// =======================================================
// Send/Recv 끝까지 반복
// =======================================================
int CConnectSocket::SendAll(const void* buf, int len)
{
    const char* p = static_cast<const char*>(buf);
    int total = 0;
    while (total < len)
    {
        int n = Send(p + total, len - total);
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

// =======================================================
// 소켓 종료
// =======================================================
void CConnectSocket::OnClose(int nErrorCode)
{
    ShutDown();
    Close();
    CSocket::OnClose(nErrorCode);

    AfxMessageBox(L"ERROR: Disconnected from server");
    ::PostQuitMessage(0);
}

// =======================================================
// 수신 이벤트
// =======================================================
void CConnectSocket::OnReceive(int nErrorCode)
{
    
    BYTE buffer[4096] = { 0 };
    int nLen = Receive(buffer, sizeof(buffer));
    if (nLen <= 0) { CSocket::OnReceive(nErrorCode); return; }

    // 1) 파일인지 체크 (헤더가 전부 왔을 때만)
    if (nLen >= (int)sizeof(FileHeader))
    {
        FileHeader* h = (FileHeader*)buffer;
        if (h->type == 1 && h->fileSize > 0)
        {
            HandleReceivedFile(buffer, nLen);
            CSocket::OnReceive(nErrorCode);
            return;
        }
    }

    // 2) 나머지는 전부 채팅으로 처리 (길이가 헤더보다 작아도!)
    if (nLen + 2 <= (int)sizeof(buffer)) {
        buffer[nLen] = 0;
        buffer[nLen + 1] = 0;
    }

    CString msg((LPCTSTR)buffer);
    CChatClientDlg* pMain = (CChatClientDlg*)AfxGetMainWnd();
    pMain->m_List.AddString(msg);
    // pMain->m_List.SetCurSel(pMain->m_List.GetCount() - 1); // 스크롤 원하면 풀기

    CSocket::OnReceive(nErrorCode);
}

// =======================================================
// 파일 보내기
// =======================================================
void CConnectSocket::SendFile(LPCTSTR lpszFilePath)
{
    CFile file;
    if (!file.Open(lpszFilePath, CFile::modeRead | CFile::typeBinary))
    {
        AfxMessageBox(L"파일 열기 실패");
        return;
    }

    FileHeader header{};
    header.type = 1;
    header.fileSize = static_cast<uint32_t>(file.GetLength());

    CString fileName = PathFindFileName(lpszFilePath);
    CT2A fileNameA(fileName, CP_UTF8);
    strcpy_s(header.fileName, _countof(header.fileName), fileNameA);

    if (SendAll(&header, sizeof(header)) == SOCKET_ERROR)
    {
        AfxMessageBox(L"헤더 전송 실패");
        file.Close();
        return;
    }

    std::vector<BYTE> buffer(1024 * 64);
    UINT bytesRead = 0;
    while ((bytesRead = file.Read(buffer.data(), (UINT)buffer.size())) > 0)
    {
        if (SendAll(buffer.data(), bytesRead) == SOCKET_ERROR)
        {
            AfxMessageBox(L"파일 전송 중 오류");
            file.Close();
            return;
        }
    }

    file.Close();
    AfxMessageBox(L"파일 전송 완료");
}

// =======================================================
// 파일 헤더 판별 (간단형: type만 체크)
// =======================================================
bool CConnectSocket::IsFileHeader(BYTE* buffer, int nLen)
{
    if (nLen < (int)sizeof(uint32_t)) return false;  // type조차 없음
    uint32_t type = *(uint32_t*)buffer;
    return (type == 1);
}

// =======================================================
// 파일 수신
//  - 헤더 덜 왔으면 여기서 RecvAll로 채움
// =======================================================
void CConnectSocket::HandleReceivedFile(BYTE* buffer, int nLen)
{
    FileHeader header{};
    int copied = std::min<int>(nLen, sizeof(FileHeader));
    memcpy(&header, buffer, copied);

    // 헤더가 덜 왔으면 여기서 채워
    if (copied < (int)sizeof(FileHeader))
    {
        int need = sizeof(FileHeader) - copied;
        if (RecvAll(((BYTE*)&header) + copied, need) <= 0)
        {
            AfxMessageBox(L"헤더 수신 실패");
            return;
        }
        buffer = nullptr;
        nLen = 0;
    }
    else
    {
        buffer += sizeof(FileHeader);
        nLen -= sizeof(FileHeader);
    }

 
    int firstChunk = nLen;
    if (firstChunk < 0) firstChunk = 0;
    if (firstChunk > (int)header.fileSize) firstChunk = header.fileSize;

    //CString savePath = L"C:\\ReceivedFiles\\";
    //::CreateDirectory(savePath, NULL);

    //CString filePath = savePath + CString(header.fileName); // Format 대신 단순 연결

    TCHAR tempDir[MAX_PATH] = { 0 };
    GetTempPath(MAX_PATH, tempDir); //윈api함수로 임시저장할 경로를 불러오는 함수(버퍼의 크기, 경로를 저장할 버퍼)
    CString tempName;
    tempName.Format(L"%s%S", tempDir, header.fileName);
    CString filePath = tempName;

    CFile file;
    if (!file.Open(filePath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
    {
        AfxMessageBox(L"파일 저장 실패");
        return;
    }

    if (firstChunk > 0 && buffer)
        file.Write(buffer, firstChunk);

    int remain = header.fileSize - firstChunk;
    std::vector<BYTE> tmp(1024 * 64);
    CString msg;
    /*msg.Format(L"[파일 저장 완료] %s (%u bytes)", filePath.GetString(), header.fileSize);
    CChatClientDlg* pMain = (CChatClientDlg*)AfxGetMainWnd();
    pMain->m_List.AddString(msg);*/

    CChatClientDlg* pMain = (CChatClientDlg*)AfxGetMainWnd();


    msg.Format(L"%S [다운로드(더블클릭)]", header.fileName);   // ex: 111.jpg [다운로드] //
    int idx = pMain->m_List.AddString(msg);          // 리스트에 추가하고 인덱스 저장
    pMain->m_FileDownloadMap[idx] = filePath;
    while (remain > 0)
    {
        int want = std::min<int>((int)tmp.size(), remain);
        int recvSize = Receive(tmp.data(), want);
        if (recvSize == SOCKET_ERROR)
        {
            file.Close();
            AfxMessageBox(L"파일 수신 오류");
            return;
        }
        if (recvSize == 0)
        {
            file.Close();
            AfxMessageBox(L"서버와 연결이 끊겼습니다.");
            return;
        }
        file.Write(tmp.data(), recvSize);
        remain -= recvSize;
    }

    file.Close();

    // 디버깅용 크기 검증
    CFileStatus st;
    if (CFile::GetStatus(filePath, st) && (uint32_t)st.m_size != header.fileSize)
    {
        CString warn;
        warn.Format(L"[경고] 저장=%lld / 헤더=%u", st.m_size, header.fileSize);
        CChatClientDlg* pMain = (CChatClientDlg*)AfxGetMainWnd();
        pMain->m_List.AddString(warn);
    }

    
}


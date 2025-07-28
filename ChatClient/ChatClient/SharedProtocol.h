#pragma once
#pragma pack(push, 1)

struct FileHeader
{
    uint32_t type;               // 1: 파일 전송
    uint32_t fileSize;           // 바이트 단위
    char fileName[256];     // 파일명
};

#pragma pack(pop)
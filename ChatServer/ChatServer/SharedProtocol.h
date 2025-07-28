#pragma once

#pragma pack(push, 1)
struct FileHeader
{
    uint32_t type;          // 1 = file
    uint32_t fileSize;      // (2GB 이상이면 uint64_t 로 바꿔)
    char     fileName[256];     // 파일명 (ASCII or UTF-8)
};
#pragma pack(pop)
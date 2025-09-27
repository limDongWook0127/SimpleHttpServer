# Simple HTTP Server (C++ / WinSock)

윈도우 WinSock2 기반으로 구현한 간단한 HTTP 서버입니다.  
원리 이해와 연습을 위한 기본 라우팅을 포함합니다.

## ✨ Features
- HTTP 요청 파싱 (메서드, 경로, 버전, 헤더, 바디)
- 라우팅 지원 (`/`, `/about`, 그 외 → 404 처리)
- 정적 파일(index.html) 응답
- 간단한 상태 코드(200, 404)와 Content-Type 처리

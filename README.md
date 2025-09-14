# RIO Server Core Library

네트워크 서버 코어 라이브러리입니다.  
Windows Registered I/O (RIO) API를 기반으로 다중 접속 환경에서 **낮은 지연 시간과 높은 처리량**을 목표로 개발되었습니다.  

---

## 주요 특징
- **Registered I/O 기반 네트워크 처리**  
- **멀티스레드 지원**  
- **커스텀 버퍼 시스템**  
  - `RIORegisterBuffer` 를 활용한 메모리 풀 관리
  - Recv/Send 요청 시 고정 크기 버퍼를 효율적으로 재사용
- **패킷 큐**  
  - `moodycamel::ConcurrentQueue`를 이용한 Lock-Free Send Queue 구현
- **패킷 직렬화/역직렬화**  
  - Google Protocol Buffers 기반 패킷 정의 및 변환

---

## 사용 라이브러리

### 1. [mimalloc](https://github.com/microsoft/mimalloc)
마이크로소프트에서 개발한 **고성능 메모리 할당자**입니다.

#### 장점
- 기본 `malloc` 대비 **메모리 단편화 감소**  
- **멀티스레드 환경에서 빠른 할당/해제 성능**  
- 소규모/빈번한 할당에서도 안정적 성능 유지  
- 별도의 코드 수정 없이 `mi_malloc`, `mi_free` API만 사용하면 교체 가능  

---

### 2. [spdlog](https://github.com/gabime/spdlog)
빠르고 간단한 C++ 로깅 라이브러리입니다.

#### 사용 목적
- 서버 동작 로그 기록 (접속, 패킷 처리, 예외 상황 등)
- 비동기 로깅 지원으로 **실시간 성능에 영향 최소화**
- 다양한 출력 형식(Console, File, Rotating File) 제공  

---

### 3. [Poco C++ Libraries](https://pocoproject.org/)
범용 C++ 애플리케이션 프레임워크로, DB 연동에 활용했습니다.

#### 사용 목적
- **Poco::Data**: 데이터베이스 ORM 스타일 접근
- **Poco::Data::ODBC**: MSSQL / MySQL 등 다양한 DB 연동 지원
- Connection Pool, Transaction, Stored Procedure 실행 등 **DB 작업 효율화**  

---

### 4. [moodycamel ConcurrentQueue](https://github.com/cameron314/concurrentqueue)
고성능 Lock-Free 멀티프로듀서/멀티컨슈머 큐입니다.

#### 장점
- **Lock-Free 구조**로 멀티스레드 환경에서 빠른 성능
- **낮은 지연 시간**으로 실시간 네트워크 패킷 처리에 적합
- Bounded / Unbounded 모드 지원 → 다양한 큐 사용 패턴에 대응
- 내부적으로 false sharing 방지, cache-friendly 구조로 설계  

---

### 5. [Google Protocol Buffers](https://protobuf.dev/)
구글에서 개발한 직렬화 라이브러리로, 네트워크 패킷 정의와 직렬화/역직렬화에 활용했습니다.

#### 사용 목적
- `.proto` 파일 기반으로 **일관된 패킷 포맷 정의**
- 이진 직렬화로 **빠르고 compact한 데이터 전송**
- 다양한 언어 지원(C++, C#, Python 등)으로 **클라이언트/서버 간 호환성 보장**
- 런타임에서 안정적인 파싱 및 에러 처리  

---

## 빌드 환경
- **OS**: Windows 10 이상
- **Compiler**: Visual Studio 2022 (MSVC)
- **Language**: C++20
- **Dependencies**: 
  - WinSock2 / Mswsock
  - mimalloc
  - spdlog
  - Poco::Data / Poco::ODBC
  - moodycamel ConcurrentQueue
  - Google Protocol Buffers

---

## 프로젝트 구조
- ServerApp : ServerCore를 활용해 개발한 간단한 예제 서버
- dummy_client : 이전 테스트에 이용했던 클라이언트 -> 리폼필요
- ServerCore : 코어 라이브러리

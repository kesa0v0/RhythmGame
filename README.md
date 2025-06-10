# Linux Console 기반 리듬 게임

## How to build

- make : 최초 빌드 시 사용
- make run: dependency update 없이 빌드 후 바로 실행
- make server: server 빌드 시 사용

## How to Run

1. server 폴더에 있는 ./server 실행
2. 메인 폴더에 있는 ./game 실행
3. 리더보드에 기록할 닉네임 입력
4. 노래 선택

## 비트맵 추가
beatmaps 폴더 아래 노래_제목.txt로 비트맵이 구현되어 있다.
```
@노래_제목 노래_길이(ms) 노래_path
#120
(비트가 판정선에 도착하는 ms) (키)
(비트가 판정선에 도착하는 ms) (키)
(비트가 판정선에 도착하는 ms) (키)
```
위 포맷을 가지고 있다.
그리고 musics 폴더 아래에 노래 파일을 두면 된다.

## 
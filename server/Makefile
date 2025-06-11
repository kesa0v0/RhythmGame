# Makefile

CC = gcc
CFLAGS = -Wall -pthread
TARGET = server
SRCS = server.c

.PHONY: all clean deps run

# 기본 make 명령: 설치 + 빌드
all: deps $(TARGET)

# 컴파일
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

# 의존성 자동 설치
deps:
	@echo "🔧 필요한 패키지를 설치 중입니다 (gcc, build-essential)..."
	sudo apt-get update
	sudo apt-get install -y gcc build-essential

# 서버 실행
run: all
	@echo "🚀 서버를 실행합니다..."
	./$(TARGET)

# 정리
clean:
	rm -f $(TARGET)

#
# Makefile
#

#最終目的のファイル
TARGET = j19415.exe

# ソースファイル(*.c)の一覧
SRCS = main.c display.c reshape.c math.c drawFunction.c color.c effect.c

# オブジェクトファイルの一覧
OBJS = ${SRCS:.c=.o} myicon.o

# ヘッダファイルの一覧
HEADERS = headers.h

# コンパイラ・リンカの指定
CC = i686-pc-cygwin-gcc
WINDRES = i686-pc-cygwin-windres
CCFLAGS = -Wall -I/usr/include/opengl
LD = $(CC)
LDFLAGS =
LIBS = -lm -lglpng -lglut32 -lglu32 -lopengl32

# OBJSからTARGETを作る方法
$(TARGET) : $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $(TARGET) $(LIBS)

myicon.o : myicon.rc
	$(WINDRES) -i myicon.rc -o myicon.o

# *.cから*.oを作る方法
.c.o :
	$(CC) $(CCFLAGS) -c $<

# */oはHEADERとMakefileの依存(これらが書き換わったときにも*.oを再構築)
$(OBJS) : $(HEADERS) Makefile

# make clean したときに実行されるコマンド
clean :
	rm -f $(TARGET) $(OBJS) core *~
	rm -f window.o






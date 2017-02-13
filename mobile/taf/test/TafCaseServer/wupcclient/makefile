CC          := gcc
CXX         := g++
CFLAGS      := -g -Wall

TAF_PATH	:= /home/kevintian/dev_taf

TARGET    	:= wupcclient
MFLAGS    	:=

INCLUDE   	:= -I${TAF_PATH}/include -I./
LIB       	:= -L${TAF_PATH}/lib -lutil



####################################################################################

JCE_SRC     := $(wildcard *.jce)
JCE_CPP     := $(patsubst %.jce, %.c, $(JCE_SRC))

LOCAL_SRC   := $(sort $(wildcard *.cpp *.c) $(JCE_CPP))
LOCAL_OBJ   := $(patsubst %.cpp, %.o, $(patsubst %.c,%.o, $(LOCAL_SRC)))

all: TafCaseServant.c ${LOCAL_OBJ} 
	g++ -o ${TARGET} ${LOCAL_OBJ} ${INCLUDE} ${LIB}

TafCaseServant.c:TafCaseServant.jce
	/home/kevintian/dev_taf/bin/jce2c TafCaseServant.jce

scp:
	myscp ${TARGET} mqq@172.27.205.110:/usr/local/app/kevintian mqq2005
	myscp boot.sh mqq@172.27.205.110:/usr/local/app/kevintian mqq2005

clean:
	rm -rf *.o ${TARGET} TafCaseServant.c TafCaseServant.h

%.o: %.cpp
	$(CXX) $(CFLAGS) $(INCLUDE) -o $@ -c $<

%.o: %.c
	$(CC)  $(CFLAGS) $(INCLUDE) -o $@ -c $<

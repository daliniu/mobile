
TOPDIR	:= ../..
include ${TOPDIR}/make.rules

#--------------------------------------------------------

INCLUDE +=
LIB	+= ${TAFLIB}

#--------------------------------------------------------

TARGET := TestCoder

#--------------------------------------------------------

#include ${TOPDIR}/inc.mk
$(TARGET):main.cpp *.jce
	../../bin/jce2cpp --coder=taf::QueryF --with-taf EndpointF.jce QueryF.jce; \
	g++ -o $(TARGET) main.cpp -I../../include/ ../../src/libutil/tc_clientsocket.o ../../src/libutil/tc_socket.o ../../src/libutil/tc_ex.o ../../src/libutil/tc_epoller.o


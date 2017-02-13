
TOPDIR	:= ../..
include ${TOPDIR}/make.rules

#--------------------------------------------------------

INCLUDE += 
LIB     += -lutil

#--------------------------------------------------------

TARGET := TestJMem

#--------------------------------------------------------

include ${TOPDIR}/inc.mk

install:
	mkdir -p ${INSTALL_PATH}/test/jmem/
	cp -rf *.cpp *.h *.jce ${INSTALL_PATH}/test/jmem/

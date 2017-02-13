TOPDIR  := ../..
include ${TOPDIR}/make.rules
#-----------------------------------------------------------------------
TARGET  := QueryStatServer

include ./makefile.target

JCEFLAGS = --with-taf

INCLUDE   += -I/usr/local/mqq/wbl/include 
LIB       += ${TAFLIB} -lmysqlclient
#-----------------------------------------------------------------------

include ${TOPDIR}/jce/querystat/QueryStatServant.mk
include ${TOPDIR}/inc.mk

#-----------------------------------------------------------------------

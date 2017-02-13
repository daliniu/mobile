

TOPDIR  := ../..
include ${TOPDIR}/make.rules

#-------------------------------------------------------------------------------

INCLUDE   += -I${TOPDIR}/include/servant
LIB       += ${TAFLIB} -lmysqlclient

#-------------------------------------------------------------------------------

J2CPP_FLAG = --with-taf
JCEFLAGS = --with-taf
TARGET  := tafPropertyDimenServer


#-------------------------------------------------------------------------------
include ${TOPDIR}/jce/property/PropertyDB.mk
include ${TOPDIR}/inc.mk

scp2test:
	myscp tafPropertyDimenServer mqq@172.27.208.171:/usr/local/app/taf/tafnode/data/taf.tafPropertyDimenServer/bin mqq2005

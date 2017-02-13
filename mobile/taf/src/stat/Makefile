
TOPDIR  := ../..
include ${TOPDIR}/make.rules

#-------------------------------------------------------------------------------

INCLUDE   += -I${TOPDIR}/include/servant
LIB       += ${TAFLIB} -lmysqlclient -lz

#-------------------------------------------------------------------------------

TARGET  := ${STATBIN}

#-------------------------------------------------------------------------------
include ${TOPDIR}/jce/stat/Stat.mk
include ${TOPDIR}/inc.mk

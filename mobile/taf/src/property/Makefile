
TOPDIR  := ../..
include ${TOPDIR}/make.rules

#-------------------------------------------------------------------------------

INCLUDE   += -I${TOPDIR}/include/servant
LIB       += ${TAFLIB} -lmysqlclient

#-------------------------------------------------------------------------------

TARGET  := ${PROPERTYBIN}

#-------------------------------------------------------------------------------
include ${TOPDIR}/jce/property/PropertyDB.mk
include ${TOPDIR}/inc.mk

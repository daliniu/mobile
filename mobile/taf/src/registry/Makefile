
TOPDIR  := ../..
include ${TOPDIR}/make.rules

#-------------------------------------------------------------------------------
INCLUDE   += -I../../include/servant
LIB       += ${TAFLIB} -lmysqlclient
#-------------------------------------------------------------------------------

TARGET  := ${REGISTRYBIN}

#-------------------------------------------------------------------------------
include ${TOPDIR}/jce/registry/Registry.mk
include ${TOPDIR}/jce/node/Node.mk
include ${TOPDIR}/inc.mk

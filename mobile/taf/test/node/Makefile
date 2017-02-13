
TOPDIR  := ../..
include ${TOPDIR}/make.rules

#-------------------------------------------------------------------------------
INCLUDE   += -I${TOPDIR}/include/servant -I${TOPDIR}/test/server 
LIB       += ${TAFLIB} ${TOPDIR}/test/server/Server.o

#-------------------------------------------------------------------------------

TARGET  := TestNode

#-------------------------------------------------------------------------------

include ${TOPDIR}/jce/stat/Stat.mk
include ${TOPDIR}/jce/node/Node.mk
include ${TOPDIR}/inc.mk
include ${TOPDIR}/jce/registry/Registry.mk


#-----------------------------------------------------------------------

APP       := QUAN
TARGET    := CircleServer
MFLAGS    :=
CONFIG    := 
STRIP_FLAG:= N
J2CPP_FLAG:= 


#-----------------------------------------------------------------------

include /home/tafjce/QUAN/SecretServer/SecretServer.mk
include /home/tafjce/MDW/UniqIDServer/UniqIDServer.mk
include /home/zhengqiao/Util/ErrorCode/ErrorCode.mk
include /home/zhengqiao/Util/StringUtil/StringUtil.mk
include /home/zhengqiao/Util/LogHelper/LogHelper.mk
include /home/zhengqiao/Util/LockedSingleton/LockedSingleton.mk
include /home/zhengqiao/Util/Var/Var.mk
include /usr/local/taf/makefile.taf
#JCE2CPP     := /home/zhengqiao/taf-2012/bin/jce2cpp ${J2CPP_FLAG}
LIB       += -lmysqlclient -lz -lrt

#-----------------------------------------------------------------------

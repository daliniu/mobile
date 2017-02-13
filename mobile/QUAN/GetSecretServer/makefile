
#-----------------------------------------------------------------------

APP       := QUAN
TARGET    := GetSecretServer
MFLAGS    :=
CONFIG    := 
STRIP_FLAG:= N
J2CPP_FLAG:= 

INCLUDE   += -I/usr/local/mqq/wbl/include 
LIB       += -L/usr/local/mqq/wbl/lib 

#-----------------------------------------------------------------------
#INCLUDE += -I/home/zhangys/tmp/boost_1_55_0
include /home/tafjce/QUAN/CircleServer/CircleServer.mk
include /home/tafjce/QUAN/LbsServer/LbsServer.mk
include /home/tafjce/QUAN/SecretServer/SecretServer.mk
include /home/tafjce/QUAN/PostServer/PostServer.mk
include /home/tafjce/QUAN/UserInfoServer/UserInfoServer.mk
include /usr/local/taf/makefile.taf
LIB       += -lmysqlclient -lz 
#-----------------------------------------------------------------------

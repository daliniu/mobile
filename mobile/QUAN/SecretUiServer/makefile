
#-----------------------------------------------------------------------

APP       := QUAN
TARGET    := SecretUiServer
MFLAGS    :=
CONFIG    := 
STRIP_FLAG:= N
J2CPP_FLAG:= 

INCLUDE   += -I/usr/local/mqq/wbl/include 
LIB       += -L/usr/local/mqq/wbl/lib 

#-----------------------------------------------------------------------
include /home/zhengqiao/Util/BatchHttp/BatchHttp.mk
include /home/tafjce/QUAN/SecretServer/SecretServer.mk
include /home/tafjce/QUAN/LbsServer/LbsServer.mk
include /home/tafjce/QUAN/PostServer/PostServer.mk
include /home/tafjce/QUAN/GetSecretServer/GetSecretServer.mk
include /home/tafjce/QUAN/UserInfoServer/UserInfoServer.mk
include /home/tafjce/QUAN/MsgServer/MsgServer.mk
include /usr/local/taf/makefile.taf

#-----------------------------------------------------------------------

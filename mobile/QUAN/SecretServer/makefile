
#-----------------------------------------------------------------------

APP       := QUAN
TARGET    := SecretServer
MFLAGS    :=
CONFIG    := 
STRIP_FLAG:= N
J2CPP_FLAG:= 

INCLUDE   += -I/usr/local/mqq/wbl/include 
LIB       += -L/usr/local/mqq/wbl/lib 

#-----------------------------------------------------------------------
include /home/tafjce/QUAN/UserInfoServer/UserInfoServer.mk
include /home/tafjce/QUAN/MsgServer/MsgServer.mk
include /usr/local/taf/makefile.taf
LIB       += -lmysqlclient -lz 
#-----------------------------------------------------------------------
rc:
	cp -rf Base.h $(SERVER_DIR)

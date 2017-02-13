
#-----------------------------------------------------------------------

APP       := Test
TARGET    := TafCaseClient
MFLAGS    :=
CONFIG    := 
STRIP_FLAG:= N
J2CPP_FLAG:= 

INCLUDE   	+= -I/usr/local/mqq/wbl/include
LIB       	+= -L/usr/local/mqq/wbl/lib -lwbl


#-----------------------------------------------------------------------

include /home/tafjce/Test/TafCaseServer/TafCaseServer.mk
TAF_PATH:=/home/kevintian/dev_taf
include ${TAF_PATH}/makefile.taf

scp:
	myscp TafCaseClient mqq@172.27.205.110:/usr/local/app/kevintian mqq2005
	

#-----------------------------------------------------------------------

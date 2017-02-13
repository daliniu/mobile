APP       	:= Test
TARGET		:= wupclient
MFLAGS    	:= 32
CONFIG    	:= 
STRIP_FLAG	:= Y


INCLUDE   	+= 
LIB       	+= 


include /home/tafjce/Test/TafCaseServer/TafCaseServer.mk

TAF_PATH	:= /home/kevintian/dev_taf
include ${TAF_PATH}/makefile.taf

scp:
	myscp ${TARGET} mqq@172.27.205.110:/usr/local/app/kevintian mqq2005
APP       	:= Test
TARGET		:= KevinTestClient
MFLAGS    	:=
CONFIG    	:= 
STRIP_FLAG	:= Y


INCLUDE   	+= -I../server/ 
LIB       	+= 
REMOTE_OBJ	+= ../server/KevinTestServant.o

TAF_PATH:=/home/kevintian/dev_taf/Taf_trunk
include ${TAF_PATH}/makefile.taf

#include /usr/local/taf/makefile.taf

scp:
	myscp KevinTestClient mqq@172.27.205.110:/usr/local/app/kevintian mqq2005

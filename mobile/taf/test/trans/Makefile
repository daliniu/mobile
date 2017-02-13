INC_TRANS 	:= -I../lib/
LIB_TRANS 	:= ../lib/libtrans.a

INCLUDE		:= $(INC_TRANS) -I./
LIB			:= $(LIB_TRANS)

all:main

main:struct.h util/tc_file.o util/tc_common.o util/tc_ex.o main.o 
	g++    -o main main.o util/tc_file.o util/tc_common.o  util/tc_ex.o $(INCLUDE) $(LIB)

main.o:main.cpp
	g++ -c -o main.o main.cpp $(INCLUDE) 

struct.h:struct.jce
	/home/kevintian/dev_taf/bin/jce2cpp struct.jce

util/tc_file.o:util/tc_file.cpp
	g++ -c -o util/tc_file.o util/tc_file.cpp $(INCLUDE)

util/tc_common.o:util/tc_common.cpp
	g++ -c -o util/tc_common.o util/tc_common.cpp $(INCLUDE)

util/tc_ex.o:util/tc_ex.cpp
	g++ -c -o util/tc_ex.o util/tc_ex.cpp $(INCLUDE)	

clean:
	rm -rf ../*.o util/*.o *.o struct.h main
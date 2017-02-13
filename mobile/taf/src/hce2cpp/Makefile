TOPDIR	:= ../..
include ${TOPDIR}/make.rules

INCLUDE +=
LIB     += -lutil

YACC_Y	:= hce.y
LEX_L	:= hce.l

YACC_H 	:= hce.tab.hpp
YACC_C 	:= hce.tab.cpp
YACC_O	:= hce.tab.o
LEX_C	:= lex.yy.cpp
LEX_O	:= lex.yy.o

#------------------------------------------------------------------------------
INCLUDE 	+=
LIB     	+= -lutil
LOCAL_OBJ	+= ${LEX_O} ${YACC_O} 
CLEANFILE   += ${YACC_H} ${YACC_C} ${LEX_C}

TARGET		:= ${HCE2CPP}

all : ${YACC_H} ${LEX_C} ${TARGET}

#------------------------------------------------------------------------------
include ${TOPDIR}/inc.mk


${YACC_H} ${YACC_C} : ${YACC_Y}
	rm -vf $(YACC_H) $(YACC_C) $(YACC_O)
	${YACC} $^ -o ${YACC_C}

${LEX_C} : ${LEX_L}
	rm -vf ${LEX_C} ${LEX_O}
	${LEX} $^

install:
	echo "do nothing"

#-------------------------------------------------------------------------------


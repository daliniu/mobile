CC=gcc
TARGET  = demo
CFLAGS+= -g -Wall $(JMP) 
JCE2C =/usr/local/taf/bin/jce2c

JCE_SRC := $(wildcard *.jce)
JCE_H   := $(patsubst %.jce,%.h, $(JCE_SRC))
JCE_C    := $(patsubst %.jce,%.c, $(JCE_SRC))
LOCAL_SRC   	+= $(sort $(wildcard *.cpp *.c) $(JCE_C))
LOCAL_OBJ   	+= $(patsubst %.cpp,%.o, $(patsubst %.c,%.o, $(LOCAL_SRC)))

$(TARGET): $(JCE_H) $(JCE_C) $(LOCAL_OBJ)
	$(CC) -o $@  $(LOCAL_OBJ)

ifneq ($(JCE_SRC),)
$(JCE_H) $(JCE_C) : $(JCE_SRC)
	rm -vf $(JCE_H) $(JCE_C) $(LOCAL_OBJ)
	$(JCE2C) ${JCEFLAGS} $(JCE_SRC)
endif

.%.d: %.cpp
	@echo "update $@ ..."; \
	echo -n $< | sed s/\.cpp/\.o:/ > $@; \
	$(CC) $(INCLUDE) -MM $< | sed '1s/.*.://' >> $@;

%.o: %.cpp
	$(CXX) -m$(MFLAGS) $(CFLAGS) $(INCLUDE) -o $@ -c $<

.%.d: %.c
	@echo "update $@ ..."; \
	echo -n $< | sed s/\.c/\.o:/ > $@; \
	$(CC) $(INCLUDE) -MM $< | sed '1s/.*.://' >> $@;

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

clean:
	@echo "cleanall"; \
	rm -vf $(TARGET) *.o .*.d.tmp .*.d gmon.out core.* *.log $(JCE_H) $(JCE_C) $(LOCAL_OBJ)

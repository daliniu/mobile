APP=Java
TARGET=Test
PATCHS=classes lib

include ./makefile.target

TARGET:
	@/bin/sh build.sh

tar: TARGET 
	@if [ -d $(TARGET)_tmp_dir ]; then \
		echo "dir has exist:$(TARGET)_tmp_dir, abort."; \
		exit 1; \
	else \
		mkdir $(TARGET)_tmp_dir $(TARGET)_tmp_dir/$(TARGET);\
		cp -rf $(PATCHS) $(TARGET)_tmp_dir/$(TARGET)/; \
		cd $(TARGET)_tmp_dir; tar cvfz $(TARGET).tgz $(TARGET)/; cd ..; \
		if [ -f "$(TARGET).tgz" ]; then \
			mv -vf $(TARGET).tgz $(TARGET).`date +%Y%m%d%H%M%S`.tgz; \
		fi; \
		mv $(TARGET)_tmp_dir/$(TARGET).tgz ./; \
		rm -rf $(TARGET)_tmp_dir; \
		echo "tar cvfz $(TARGET).tgz ..."; \
	fi

upload: tar
	@echo "$(TARGET).tgz --- $(APP).$(TARGET).tgz"
	@myscp $(TARGET).tgz mqq@172.27.34.213:/usr/local/app/patch2test/$(APP).$(TARGET).tgz mqq2005
	@wget -O /tmp/$(APP).$(TARGET).wget.out "http://172.27.34.213:8080/pages/taf_server/doUpload.xjsp?SERVER=$(APP).$(TARGET)&TGZ=/usr/local/app/patch2test/$(APP).$(TARGET).tgz"
	@cat /tmp/$(APP).$(TARGET).wget.out

upload2test: tar
	@echo "$(TARGET).tgz --- $(APP).$(TARGET).tgz"
	@myscp $(TARGET).tgz mqq@172.27.194.147:/usr/local/app/patch2test/$(APP).$(TARGET).tgz mqq2005
	@wget -O /tmp/$(APP).$(TARGET).wget.out "http://172.27.194.147:8080/pages/taf_server/doUpload.xjsp?SERVER=$(APP).$(TARGET)&TGZ=/usr/local/app/patch2test/$(APP).$(TARGET).tgz"
	@cat /tmp/$(APP).$(TARGET).wget.out
	
release: TARGET
	@/bin/sh jarjce.sh
	
code:
	@/bin/sh genjce.sh

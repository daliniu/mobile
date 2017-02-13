#[20120522 v2.2.0]
#1 实现了eto_s接口和_stoe接口 用于转换不带前缀的枚举字符串
#2 修改isJceEnumStringEqual函数，去掉第三个参数prefixLength
#3 增加WirelessUnifiedProtocol-Debug.h头文件，用于Debug和容错相关的宏定义
#4 用Jce编译器生成的代码更新Mtt协议
#5 配合代码生成工具实现对无jce_前缀的getter方法和setter方法的支持
#6 增加enum类型对应的实现etos和stoe的支持类的宏开关
#7 增加Jce类型定义和Jce默认值常量定义
#[20120522 v2.1.0]
#1 用Jce编译器生成的代码更新Mtt协议
#2 增加Jce类型定义和Jce默认值常量定义
#[20120508 v1.0.2]
#1 最新的ObjC的WUP框架和示例代码，修复了内存泄漏的bug，改善了RMI模型 by renjunyi

VERSION=2.2.0

TARGET=wup.objc_$(VERSION)

FILES :=`ls -I "makefile" -I "$(TARGET)_tmp_dir"`

tar: clean
	@if [ -d $(TARGET)_tmp_dir ]; then \
		echo "dir has exist:$(TARGET)_tmp_dir, abort."; \
		exit 1; \
	else \
		mkdir -p $(TARGET)_tmp_dir/$(TARGET);\
		echo $(FILES);\
		cp -rf $(FILES) $(TARGET)_tmp_dir/$(TARGET)/; \
		cd $(TARGET)_tmp_dir; tar --exclude=".svn" --exclude="_svn" -czvf $(TARGET).tgz $(TARGET)/; cd ..; \
		if [ -f "$(TARGET).tgz" ]; then \
			mv -vf $(TARGET).tgz $(TARGET).`date +%Y%m%d%H%M%S`.tgz; \
		fi; \
		mv $(TARGET)_tmp_dir/$(TARGET).tgz ./; \
		rm -rf $(TARGET)_tmp_dir; \
		echo "tar cvfz $(TARGET).tgz ..."; \
	fi
clean:
	-@rm *.tgz
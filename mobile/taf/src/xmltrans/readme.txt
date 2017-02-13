--------
该目录编译后将生成一个库文件:libxmltrans.a。用于将使用XML格式描述的协议，转换成WUP协议数据流。目前多在Iphone平台上使用。

可以直接在TAF框架内使用，也可以提供包给终端使用。
km上有个开发包，请注意更新。

--------
向外提供该软件包的时候需要注意，还需要TAF里的一些头文件，最终软件包含有文件目录如下：
jce/JceDisplayer.h
jce/Jce.h
jce/JceType.h
jce/RequestF.h
Markup.h
Markup.cpp
xmltrans.h
xmltrans.cpp
Makefile (该文件是下一节所示的文件内容，并非当前目录下先存在的Makefile)

---------
提供给使用者的Makefile文件内容，使用该Makefile将编译成一个.a文件：

INCLUDE := -I./

all:libxmltrans.a

libtrans.a:xmltrans.o Markup.o
        ar r libtrans.a xmltrans.o Markup.o

Markup.o:Markup.cpp
        g++ -Wall -c -o Markup.o Markup.cpp -I./ $(INCLUDE)

xmltrans.o:xmltrans.cpp
        g++ -Wall -c -o xmltrans.o xmltrans.cpp -I./ $(INCLUDE)

clean:
        rm -rf *.o libxmltrans.a
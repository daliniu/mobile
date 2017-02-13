1、/src目录下是wup、taf-proxy、taf-server三者共同的代码基。
2、wup和taf-proxy相互依赖、taf-server依赖wup和taf-proxy。
3、/ant目录下是相关的构建脚本，构建出的包会输出到/doc目录下。
4、每次构建会删除并重建/ant/build目录，这个build目录请添加到svn忽略列表中。
5、源码提交时请填写svn注释，在源码提交的同时不要忘记构建相应的包并提交。
6、注意每周要检查下老svn目录的源码更改，及时与其代码保持一致。
7、老svn目录是https://tc-svn.tencent.com/mqq/mqq_wsrdplat_rep/Taf_proj/trunk/java。
8、原有的mina在部分的客户端还有使用，因此这类工程引用nettool jar的时候，需要把taf的nettool改名称

【性能测试】
环境10.137.152.169，服务在10.137.152.170上，Taf-Java同步调用，发送包在64字节左右
CPU:Intel(R) Xeon(R) CPU X3440  @ 2.53GHz*8
Java Server 1024M/1024M/800M/800M Parallel
Java Client	1024M/1024M/800M/800M Parallel

【Server】网络线程默认为9
Proxy进程数*线程数		同步		 性能req/s   服务线程
1					Y		9738		10
2					Y		16528		10
4					Y		28268		10
8					Y		31809		10
2*8					Y		67324		10
4*8					Y		103889		10
5*8					Y		114286		10
6*8					Y		115087		10
4*16				Y		131178		32
6*16				Y		145419		32
两台机器没有修改cpu调度方式，4进程以上的客户端分布在服务上，以上测试无错误发生



Java Server 1024M/1024M/800M/800M Parallel
Java Client	1024M/1024M/800M/800M Parallel
【Proxy】网络线程=5，业务处理线程不受限
(客户端、服务端均部署在10.137.152.170)
Proxy线程数			同步		性能req/s
1					Y		9738
2					Y		18691
4					Y		33404
8					Y		53601	
16					Y		65790	
64					Y		97089	


----------
6*16测试用例情况下
客户端
top - 15:23:24 up 125 days,  1:09,  1 user,  load average: 18.24, 16.00, 10.91
Tasks: 123 total,   1 running, 122 sleeping,   0 stopped,   0 zombie
Cpu0  : 32.6%us, 60.8%sy,  0.0%ni,  3.7%id,  0.0%wa,  0.0%hi,  3.0%si,  0.0%st
Cpu1  : 31.9%us, 54.5%sy,  0.0%ni, 10.3%id,  0.0%wa,  0.0%hi,  3.3%si,  0.0%st
Cpu2  : 18.6%us, 26.6%sy,  0.0%ni,  3.0%id,  0.0%wa,  0.0%hi, 51.8%si,  0.0%st
Cpu3  : 54.3%us, 32.8%sy,  0.0%ni,  8.6%id,  0.0%wa,  0.0%hi,  4.3%si,  0.0%st
Cpu4  : 30.8%us, 60.6%sy,  0.0%ni,  5.0%id,  0.0%wa,  0.0%hi,  3.6%si,  0.0%st
Cpu5  : 32.9%us, 54.6%sy,  0.0%ni,  9.5%id,  0.0%wa,  0.0%hi,  3.0%si,  0.0%st
Cpu6  : 29.2%us, 53.5%sy,  0.0%ni, 16.3%id,  0.0%wa,  0.0%hi,  1.0%si,  0.0%st
Cpu7  : 37.1%us, 51.0%sy,  0.0%ni,  7.9%id,  0.0%wa,  0.0%hi,  4.0%si,  0.0%st
Mem:   8163908k total,  7494504k used,   669404k free,   461924k buffers
Swap:  2104504k total,        0k used,  2104504k free,  1469688k cached

服务端
top - 15:23:50 up 125 days,  1:10,  1 user,  load average: 4.61, 4.47, 3.04
Tasks: 120 total,   1 running, 119 sleeping,   0 stopped,   0 zombie
Cpu0  : 22.3%us, 12.3%sy,  0.0%ni, 64.3%id,  0.0%wa,  0.0%hi,  1.0%si,  0.0%st
Cpu1  : 22.3%us, 12.6%sy,  0.0%ni, 63.5%id,  0.0%wa,  0.0%hi,  1.7%si,  0.0%st
Cpu2  : 18.9%us, 20.3%sy,  0.0%ni, 19.3%id,  0.0%wa,  0.0%hi, 41.5%si,  0.0%st
Cpu3  : 24.7%us,  9.0%sy,  0.0%ni, 64.0%id,  0.0%wa,  0.0%hi,  2.3%si,  0.0%st
Cpu4  : 20.3%us, 13.0%sy,  0.0%ni, 65.3%id,  0.0%wa,  0.0%hi,  1.3%si,  0.0%st
Cpu5  : 21.6%us, 15.0%sy,  0.0%ni, 62.1%id,  0.0%wa,  0.0%hi,  1.3%si,  0.0%st
Cpu6  : 21.7%us, 17.3%sy,  0.0%ni, 56.7%id,  0.0%wa,  0.0%hi,  4.3%si,  0.0%st
Cpu7  : 23.3%us, 11.3%sy,  0.0%ni, 64.1%id,  0.0%wa,  0.0%hi,  1.3%si,  0.0%st
Mem:   8163908k total,  2835392k used,  5328516k free,   454824k buffers
Swap:  2104504k total,        0k used,  2104504k free,  1258852k cached
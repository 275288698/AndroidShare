#define eprintf(...) fprintf (stderr, __VA_ARGS__)

#define Conn(x,y) x##y
#define ToChar(x) #@x
#define ToString(x) #x

int n = Conn(123,456); ==> int n=123456;
char* str = Conn("asdf", "adf");==> char* str = "asdfadf";

char a = ToChar(1);==> char a='1';
char a = ToChar(123); ==> char a='3';如果参数超过四个字符,报错了！error C2015: too many characters in constant ：P
         
char* str = ToString(123132);==> char* str="123132"; 
             

printf("i said: assert failed:'%s', %s:%u",#e,__FILE__,__LINE__)

＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
//	while((cnt=fread(buff,1,BUFF_SIZE,sourceP))>0 ){
	//		int writeCnt = fwrite(buff,1,cnt,destP);
	//		printf("%s,---%d---%d\n ",buff,cnt,writeCnt);
	//	}
		memset(buff, 0, sizeof(buff));
		while(!feof(sourceP)){
			fgets(buff,BUFF_SIZE,sourceP);
			fputs(buff,destP);
			printf("\n============%s",buff);
	//		memset(buff, 0, sizeof(buff));
		}
＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
main    a
		b
		func_b
		func_a
		_____
		_____(函数返回地址＋返回信息)		
func1	a
		b
		
gets(char *); //unsafe
	
	
	函数（程序自身）
	字符串常量
	－－－－－
	静态变量
	（函数内的static变量＋文件内的static变量＋全局变量）
	－－－－－－
	利用malloc分配的内存区域
	－－－－－－－
	通过brk（）伸长
		｜
		｜
	调用函数伸长
	－－－－－－－－－－－
	自动变量
	
	
		
＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
符号表 ： nm 
还可以给出在原程序中这个函数（符号）是在多少行定义的，不过这必须要求编译该函数库的时候加“-l”选项。 
符号的类型是以一个字母的形式显示的，小写字母表示这个符号是本地（local）的，而大写字母则表示这个符号是全局的（global,externel）。

D表示时初始化过的数据段；
B表示未初始化的数据段；

T表示在代码段中定义的一般变量符号；
U表示没有定义的，在这个库里面使用了，但是在其他库中定义的符号；
W，weak的缩写，表示如果其他函数库中也有对这个符号的定义，则其他符号的定义可以覆盖这个定义。

===============================
char * s1 = "hello world";
char s2[] = "hello world";
strtok;
strcpy;
strcat;
strncyp;
memset;
memmove;
memcpy
================================

unsigned char cBuff[len];
(*env)->GetByteArrayRegion(env,jbyteArr,0,len,cBuff);


unsigned char cBuff = *(*env)->GetByteArrayElements(env,jbyteArr,NULL);
(*env)->ReleaseByteArrayElements(env,jbyteArr,cBuff,0);
---------------------------------------
way 1:
jbyteArray jbyteArr = (*env)->NewByteArray(env,len);
(*env)->SetByteArrayElements(env,jbyteArr,0,len,cBuff);
return jbyteArr;

way 2: pass the cBuff through the jniCallBack

way 3: DirectByteBuff

for java:
	public class Native{
		private ByteBuffer mDirectBuffer;
		public native void nativeSetDirectBuffer(Object buffer, int len);
		
		public Native(){mDirectBuffer = ByteBuffer.allocateDirect(368*640)};
	
	}

for jni:
	****_nativeSetDirectBuffer(JNIEnv *env,jobject thiz, jobject buffer, jint len){
		unsigned char * cBuff = (unsigned char *)(*env)->GetDirectBufferAddress(env,buffer);
	}



/*     基于到存储器地址的指针以及存储器长度（容量），函数分配并且返回一个新的Java.nio.ByteBuffer。如果函数没有针对当前Java虚拟机实现，则返回NULL，或者抛出一个异常。如果没有存储器可用，则将会抛出一个OutOfMemoryException。*/

jobject NewDirectByteBuffer(void* address, jlong capacity);

/*   GetDirectBufferAddress函数返回一个指向被传入的java.nio.ByteBuffer对象的地址指针。如果函数尚未针对当前虚拟机实现，或者如果buf不是java.nio.ByteBuffer的一个对象，又或者存储器区尚未定义，则都将返回NULL。*/

void* GetDirectBufferAddress(jobject buf);

/*GetDirectBufferCapacity函数返回被传入的java.nio.ByteBuffer对象的容量（以字节计数）。如果函数没有针对当前环境实现，或者如果buf不是java.nio.ByteBuffer类型的对象返回-1。*/

jlong GetDirectBufferCapacity(jobject buf);




===========================
gcc foo.c -Wl,-Bstatic -lbar -lbaz -lqux -Wl,-Bdynamic -lcorge -o foo.exe

CORE_LIBS="$CORE_LIBS -L/usr/lib64/mysql -Wl,-Bstatic -lmysqlclient \ -Wl,-Bdynamic -lz -lcrypt -lnsl -lm -L/usr/lib64 -lssl -lcrypto"

===========================
gcc -shared -fPIC -o 1.so 1.c
这里有一个-fPIC参数
PIC就是position independent code
PIC使.so文件的代码段变为真正意义上的共享
如果不加-fPIC,则加载.so文件的代码段时,代码段引用的数据对象需要重定位, 重定位会修改代码段的内容,这就造成每个使用这个.so文件代码段的进程在内核里都会生成这个.so文件代码段的copy.每个copy都不一样,取决于 这个.so文件代码段和数据段内存映射的位置.

因此,不用fPIC编译so并不总是不好.
如果你满足以下4个需求/条件时，可以不使用-fPIC选项。
1.该库可能需要经常更新
2.该库需要非常高的效率(尤其是有很多全局量的使用时)
3.该库并不很大.
4.该库基本不需要被多个应用程序共享
===========================


$ gcc test_a.c test_b.c test_c.c -fPIC -shared -o libtest.so

===========================
l 测试是否动态连接，如果列出libtest.so，那么应该是连接正常了
 
$ ldd test
 
l 执行test，可以看到它是如何调用动态库中的函数的。
3、编译参数解析
最主要的是GCC命令行的一个选项:
-shared 该选项指定生成动态连接库（让连接器生成T类型的导出符号表，有时候也生成弱连接W类型的导出符号），不用该标志外部程序无法连接。相当于一个可执行文件
l -fPIC：表示编译为位置独立的代码，不用此选项的话编译后的代码是位置相关的所以动态载入时是通过代码拷贝的方式来满足不同进程的需要，而不能达到真正代码段共享的目的。
l -L.：表示要连接的库在当前目录中
l -ltest：编译器查找动态连接库时有隐含的命名规则，即在给出的名字前面加上lib，后面加上.so来确定库的名称
l LD_LIBRARY_PATH：这个环境变量指示动态连接器可以装载动态库的路径。(DYLD_LIBRARY_PATH)
l 当然如果有root权限的话，可以修改/etc/ld.so.conf文件，然后调用 /sbin/ldconfig来达到同样的目的，不过如果没有root权限，那么只能采用输出LD_LIBRARY_PATH的方法了。
4、注意
调用动态库的时候有几个问题会经常碰到，有时，明明已经将库的头文件所在目录 通过 “-I” include进来了，库所在文件通过 “-L”参数引导，并指定了“-l”的库名，但通过ldd命令察看时，就是死活找不到你指定链接的so文件，这时你要作的就是通过修改 LD_LIBRARY_PATH或者/etc/ld.so.conf文件来指定动态库的目录。通常这样做就可以解决库无法链接的问题了。


＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝＝
$ ar rc librtmp.a rtmp_s.o
(ranlib librtmp.a 可生成索引)
用 nm librtmp.a 来看里面的目标文件和导出函数（带 T 标记）。

$gcc -o librtmp.so rtmp.o -shared -fPIC

gcc -o main -Wl,-Bstatic -lrtmp -Wl,-Bdynamic -lrtmp main.c
======================================
运行静态：.h文件不能定义变量
运行动态： export DYLD_LIBRARY_PATH="../lib"
只有在gcc  -Dname=DEFINITION 的时候传递宏
=====================================


CFLAGS -I
LDFLAGS -L
LIBS -l
LDFLAGS = -L/var/xxx/lib -L/opt/MySQL/lib

LIBS = -lmysqlclient -liconv

说到这里，进一步说说LDFLAGS指定-L虽然能让链接器找到库进行链接，但是运行时链接器却找不到这个库，如果要让软件运行时库文件的路径也得到扩展，那么我们需要增加这两个库给"-Wl,R"

LDFLAGS = -L/var/xxx/lib -L/opt/mysql/lib -Wl,R/var/xxx/lib -Wl,R/opt/mysql/lib
CFLAGS 或 CPPFLAGS的用法
CPPFLAGS='-I/usr/local/libjpeg/include -I/usr/local/libpng/include'

$@ $^ $<  $?
目标文件 ：依赖文件 第一个依赖文件 比目标文件新的依赖文件







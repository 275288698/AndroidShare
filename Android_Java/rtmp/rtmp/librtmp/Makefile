VERSION=v2.4

prefix=/usr/local
CROSS_COMPILE=$(A_NDK_ROOT)/toolchains/arm-linux-androideabi-4.9/prebuilt/darwin-x86_64/bin/arm-linux-androideabi-
DESTDIR=$(A_NDK_ROOT)/

incdir=/include/librtmp
bindir=/bin
libdir=/lib
mandir=/man
BINDIR=$(DESTDIR)$(bindir)
INCDIR=$(DESTDIR)$(incdir)
LIBDIR=$(DESTDIR)$(libdir)
MANDIR=$(DESTDIR)$(mandir)

SYSROOT=$(A_NDK_ROOT)/platforms/android-19/arch-arm
CC=$(CROSS_COMPILE)gcc --sysroot=$(SYSROOT)
LD=$(CROSS_COMPILE)ld
AR=$(CROSS_COMPILE)ar


#LIB_STASTIC=-lstastic

LIB_STASTIC=

SYS=android
#SYS=posix
CRYPTO=POLARSSL
#CRYPTO=OPENSSL
#CRYPTO=GNUTLS
DEF_POLARSSL=-DUSE_POLARSSL
DEF_OPENSSL=-DUSE_OPENSSL
DEF_GNUTLS=-DUSE_GNUTLS
DEF_=-DNO_CRYPTO
REQ_GNUTLS=gnutls,hogweed,nettle
REQ_OPENSSL=libssl,libcrypto
PUB_GNUTLS=-lgmp
LIBZ=-lz
LIBS_posix=
LIBS_darwin=
LIBS_mingw=-lws2_32 -lwinmm -lgdi32
LIB_GNUTLS=-lgnutls -lhogweed -lnettle -lgmp $(LIBZ)
LIB_OPENSSL=-lssl -lcrypto $(LIBZ)
LIB_POLARSSL=-lpolarssl $(LIBZ) -L ../3rdlib/
PRIVATE_LIBS=$(LIBS_$(SYS))

THREADLIB_posix=-lpthread
THREADLIB_darwin=-lpthread
THREADLIB_mingw=
THREADLIB=$(THREADLIB_$(SYS))

CRYPTO_LIB=$(LIB_$(CRYPTO)) $(PRIVATE_LIBS) $(LIB_STASTIC) $(THREADLIB) -lm
CRYPTO_REQ=$(REQ_$(CRYPTO))
CRYPTO_DEF=$(DEF_$(CRYPTO))
PUBLIC_LIBS=$(PUB_$(CRYPTO))

SO_VERSION=1
####
SOX_android=so
SOX_posix=so
SOX_darwin=dylib
SOX_mingw=dll
SOX=$(SOX_$(SYS))
####
SO_android=.$(SOX)
SO_posix=.$(SOX).$(SO_VERSION)
SO_darwin=.$(SO_VERSION).$(SOX)
SO_mingw=-$(SO_VERSION).$(SOX)
SO_EXT=$(SO_$(SYS))
####
SODIR_android=$(LIBDIR)
SODIR_posix=$(LIBDIR)
SODIR_darwin=$(LIBDIR)
SODIR_mingw=$(BINDIR)
SODIR=$(SODIR_$(SYS))

SO_LDFLAGS_android=-shared -Wl,-soname,$@
SO_LDFLAGS_posix=-shared -Wl,-soname,$@
SO_LDFLAGS_darwin=-dynamiclib -twolevel_namespace -undefined dynamic_lookup \
 -fno-common -headerpad_max_install_names -install_name $(libdir)/$@
SO_LDFLAGS_mingw=-shared -Wl,—out-implib,librtmp.dll.a
SO_LDFLAGS=$(SO_LDFLAGS_$(SYS))

INSTALL_IMPLIB_posix=
INSTALL_IMPLIB_darwin=
INSTALL_IMPLIB_mingw=cp librtmp.dll.a $(LIBDIR)
INSTALL_IMPLIB=$(INSTALL_IMPLIB_$(SYS))

SHARED=yes
SODEF_yes=-fPIC
SOLIB_yes=librtmp$(SO_EXT)
SOINST_yes=install_so
SO_DEF=$(SODEF_$(SHARED))
SO_LIB=$(SOLIB_$(SHARED))
SO_INST=$(SOINST_$(SHARED))

DEF=-DRTMPDUMP_VERSION=\"$(VERSION)\" $(CRYPTO_DEF) $(XDEF)
OPT=-O2
CFLAGS=-Wall $(XCFLAGS)  -I.. -I../3rdinclude/polarssl -I../3rdinclude $(DEF) -g $(OPT) $(SO_DEF) -L../3rdlib
LDFLAGS=$(XLDFLAGS)


OBJS=rtmp.o cJSON.o log.o amf.o hashswf.o parseurl.o rtmp_aac.o Rtmp_H264.o rtmp_metadata.o rtmp_protocol.o rtmp_thread.o  rtmp_buffer.o thread.o rtmp_ios.o

all:	librtmp.a $(SO_LIB)

clean:
	rm -f *.o *.a *.so

librtmp.a: $(OBJS)
	$(AR) rs $@ $?

librtmp$(SO_EXT): $(OBJS)
	$(CC) $(SO_LDFLAGS) $(LDFLAGS) -o $@ $^ $> $(CRYPTO_LIB)

log.o: log.c log.h Makefile
rtmp.o: rtmp.c rtmp.h rtmp_sys.h handshake.h dh.h log.h amf.h cJSON.h Makefile
cJSON.o: cJSON.c cJSON.h Makefile
amf.o: amf.c amf.h bytes.h log.h Makefile
hashswf.o: hashswf.c http.h rtmp.h rtmp_sys.h Makefile
parseurl.o: parseurl.c rtmp.h rtmp_sys.h log.h Makefile
thread.o: thread.c thread.h Makefile
rtmp_buffer.o: rtmp_buffer.c rtmp_buffer.h thread.h Makefile
rtmp_aac.o: rtmp_aac.c rtmp_aac.h rtmp.h rtmp_sys.h rtmp_buffer.h Makefile
Rtmp_H264.o: Rtmp_H264.c Rtmp_H264.h rtmp.h rtmp_sys.h rtmp_buffer.h Makefile
rtmp_metadata.o: rtmp_metadata.c rtmp_metadata.h rtmp.h rtmp_sys.h Makefile
rtmp_protocol.o: rtmp_protocol.c rtmp_protocol.h rtmp.h rtmp_sys.h rtmp_buffer.h Makefile
rtmp_thread.o: rtmp_thread.c rtmp_thread.h rtmp.h Rtmp_H264.h rtmp_aac.h rtmp_metadata.h rtmp_sys.h rtmp_buffer.h Makefile
rtmp_ios.o: rtmp_ios.c rtmp_ios.h log.h rtmp.h rtmp_buffer.h Makefile

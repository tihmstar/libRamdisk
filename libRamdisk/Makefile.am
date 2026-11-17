AM_CFLAGS = -I$(top_srcdir)/include $(GLOBAL_CFLAGS) $(libgeneral_CFLAGS)
AM_CXXFLAGS = $(AM_CFLAGS) $(GLOBAL_CXXFLAGS)
AM_LDFLAGS = $(libgeneral_LIBS)

lib_LTLIBRARIES = libramdisk.la

libramdisk_la_CFLAGS = $(AM_CFLAGS)
libramdisk_la_CXXFLAGS = $(AM_CXXFLAGS)
libramdisk_la_LDFLAGS = $(AM_LDFLAGS)
libramdisk_la_SOURCES = Ramdisk_libdmg.cpp
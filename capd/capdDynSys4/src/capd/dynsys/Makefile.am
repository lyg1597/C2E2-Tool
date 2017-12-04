include ${capdMake}/make/common_makefile.mkf

lib_LIBRARIES=libcapddynsys.a
libcapddynsys_a_SOURCES= DynSys.cpp  FadTaylor.cpp Taylor.cpp  TaylorHOE.cpp C2Taylor.cpp  CnTaylor.cpp   
libcapddynsys_a_CXXFLAGS= ${capddynsys_CFLAGS} ${capdAlg_CFLAGS}



pkgconfigdir = @PKG_CONFIG_DIR@
pkgconfig_DATA = capddynsysm.pc

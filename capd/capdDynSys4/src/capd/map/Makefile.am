include ${capdMake}/make/common_makefile.mkf

lib_LIBRARIES=libcapdmap.a
libcapdmap_a_SOURCES= Function.cpp  Map.cpp Parser.cpp
libcapdmap_a_CXXFLAGS= ${capdmap_CFLAGS} ${capdAlg_CFLAGS}



pkgconfigdir = @PKG_CONFIG_DIR@
pkgconfig_DATA = capdmap.pc

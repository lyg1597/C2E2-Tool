include ${capdMake}/make/common_makefile.mkf

lib_LIBRARIES=libcapdautodiff.a
libcapdautodiff_a_SOURCES= DagIndexer.cpp NodeType.cpp
libcapdautodiff_a_CXXFLAGS= ${capdautodiff_CFLAGS} ${capdAlg_CFLAGS} ${capdAux_CFLAGS}



pkgconfigdir = @PKG_CONFIG_DIR@
pkgconfig_DATA = capdautodiff.pc

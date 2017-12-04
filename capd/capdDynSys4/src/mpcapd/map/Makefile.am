include ${capdMake}/make/common_makefile.mkf

lib_LIBRARIES=libmpcapdmap.a

# TODO: remove files C2Coeff.cpp CnCoeff.cpp  Node.cpp

libmpcapdmap_a_SOURCES=  Function.cpp  Map.cpp 
libmpcapdmap_a_CXXFLAGS= ${mpcapdmap_CFLAGS} ${mpcapdAlg_CFLAGS} 

pkgconfigdir = @PKG_CONFIG_DIR@
pkgconfig_DATA = mpcapdmap.pc

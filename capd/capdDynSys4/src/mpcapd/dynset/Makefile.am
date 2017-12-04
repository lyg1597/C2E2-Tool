include ${capdMake}/make/common_makefile.mkf

lib_LIBRARIES=libmpcapddynset.a
libmpcapddynset_a_SOURCES= C0AffineSet.cpp C0DoubletonSet.cpp C0TripletonSet.cpp C0HOSet.cpp CoordReorganize.cpp FactorReorganization.cpp C1AffineSet.cpp C1DoubletonSet.cpp C11Rect2Set.cpp   CnRect2MultiMatrix.cpp CnRect2MultiLip.cpp  
libmpcapddynset_a_CXXFLAGS= ${mpcapddynset_CFLAGS} ${mpcapdAlg_CFLAGS} 



pkgconfigdir = @PKG_CONFIG_DIR@
pkgconfig_DATA = mpcapddynset.pc

include ${capdMake}/make/common_makefile.mkf

lib_LIBRARIES=libcapddynset.a
libcapddynset_a_SOURCES = C0BallSet.cpp C0AffineSet.cpp C0DoubletonSet.cpp C0HODoubletonSet.cpp C0HOSet.cpp C0TripletonSet.cpp CoordReorganize.cpp C0FlowballSet.cpp FactorReorganization.cpp C1AffineSet.cpp C1DoubletonSet.cpp C11Rect2Set.cpp C2DoubletonSet.cpp CnRect2Set.cpp CnRect2MultiLip.cpp CnRect2MultiMatrix.cpp    
libcapddynset_a_CXXFLAGS= ${capddynset_CFLAGS} ${capdAlg_CFLAGS}



pkgconfigdir = @PKG_CONFIG_DIR@
pkgconfig_DATA = capddynset.pc

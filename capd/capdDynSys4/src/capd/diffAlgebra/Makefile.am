include ${capdMake}/make/common_makefile.mkf

lib_LIBRARIES=libcapddiffAlgebra.a
libcapddiffAlgebra_a_SOURCES= C1Coeff.cpp C2Coeff.cpp CnCoeff.cpp  Curve.cpp Hessian.cpp Node.cpp SolutionCurve.cpp
libcapddiffAlgebra_a_CXXFLAGS= ${capddiffAlgebra_CFLAGS} ${capdAlg_CFLAGS}



pkgconfigdir = @PKG_CONFIG_DIR@
pkgconfig_DATA = capddiffAlgebra.pc

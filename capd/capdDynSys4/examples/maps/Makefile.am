include ${capdMake}/make/common_makefile.mkf

noinst_PROGRAMS=mapExample hessianExample jetExample

mapExample_SOURCES=mapExample.cpp
mapExample_CXXFLAGS=${capdDynSys_CFLAGS}
mapExample_libs=${capdDynSys_LIBS}
mapExample_LDADD=${mapExample_libs}
mapExample_DEPENDENCIES=${filter %.a %.la,${mapExample_libs}}

hessianExample_SOURCES=hessianExample.cpp
hessianExample_CXXFLAGS=${capdDynSys_CFLAGS}
hessianExample_libs=${capdDynSys_LIBS}
hessianExample_LDADD=${hessianExample_libs}
hessianExample_DEPENDENCIES=${filter %.a %.la,${hessianExample_libs}}

jetExample_SOURCES=jetExample.cpp
jetExample_CXXFLAGS=${capdDynSys_CFLAGS}
jetExample_libs=${capdDynSys_LIBS}
jetExample_LDADD=${jetExample_libs}
jetExample_DEPENDENCIES=${filter %.a %.la,${jetExample_libs}}

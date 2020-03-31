#add_llvm_library( TmacHello MODULE
#	Hello.cpp
#
#	PLUGIN_TOOL
#	opt
#)

#include_directories (.../include)

set (Sources
	hello_pass.cpp
)

add_library (PassObj OBJECT ${Sources})
add_library (jmpsafe SHARED $<TARGET_OBJECTS:PassObj>)

cmake_minimum_required(VERSION 3.2...3.15)

project(phigw
	VERSION 1.0
	DESCRIPTION "Hello world Phi-GW C++ Nuttx"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 99)

set(NUTTX_PATH "${CMAKE_SOURCE_DIR}/../nuttx-export-10.0.1")

include(cmake/phigw.cmake)

set(AC_COMMON_FLAGS "${AC_COMMON_FLAGS} -fno-builtin -Wall -Wshadow -Wundef -fno-strict-aliasing -fno-strength-reduce -fomit-frame-pointer -Os")
set(AC_COMMON_FLAGS "${AC_COMMON_FLAGS} -D_DEBUG -D_LIBCPP_BUILD_STATIC -D_LIBCPP_NO_EXCEPTIONS ")

set(AC_DEFINES "${AC_DEFINES} -DCONFIG_WCHAR_BUILTIN")

include_directories(
	src
	${NUTTX_PATH}/include
	${NUTTX_PATH}/include/arch
	${NUTTX_PATH}/include/libcxx
	${NUTTX_PATH}/arch/chip
)

set(EXE_NAME phigw)

set(CMAKE_C_FLAGS "${AC_HW_FLAGS} ${AC_DEFINES} ${AC_COMMON_FLAGS} ${AC_CXX_EXTRA_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-exceptions -fcheck-new -pedantic ")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -nostdinc")

set(CMAKE_CXX_FLAGS	"${AC_HW_FLAGS} ${AC_DEFINES} ${AC_COMMON_FLAGS} ${AC_CXX_EXTRA_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fcheck-new -fno-rtti -pedantic ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostdinc++")

if (PARAM_DEBUG)
    set(CMAKE_C_FLAGS     "${CMAKE_C_FLAGS} -g")
	set(CMAKE_CXX_FLAGS     "${CMAKE_CXX_FLAGS} -g")
endif()



set(CMAKE_SKIP_RPATH ON)
set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_LINKER} ${AC_LINKER_FLAGS} -o ${EXE_NAME}.elf <OBJECTS> <LINK_LIBRARIES>")

set(BUILD_SHARED_LIBS OFF)

add_subdirectory(src)


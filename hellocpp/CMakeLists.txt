cmake_minimum_required(VERSION 3.2...3.15)

project(hellocpp
	VERSION 1.0
	DESCRIPTION "Hello world C++ Nuttx"
)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)

set(NUTTX_PATH "${CMAKE_SOURCE_DIR}/nuttx-export-9.1.0")

include(cmake/phigw.cmake)

set(AC_COMMON_FLAGS "${AC_COMMON_FLAGS} -fno-builtin -Wall -Wshadow -Wundef -g") 

set(AC_DEFINES "${AC_DEFINES} -DCONFIG_WCHAR_BUILTIN")

include_directories(
	src
	${NUTTX_PATH}/include
	${NUTTX_PATH}/arch/chip
)

set(EXE_NAME hellocpp)

set(CMAKE_CXX_FLAGS	"${AC_HW_FLAGS} ${AC_DEFINES} ${AC_COMMON_FLAGS} ${AC_CXX_EXTRA_FLAGS}")
if (PARAM_DEBUG)
    set(CMAKE_CXX_FLAGS     "${CMAKE_CXX_FLAGS} -Os -g")
else()
    set(CMAKE_CXX_FLAGS     "${CMAKE_CXX_FLAGS} -Os")
endif()

set(CMAKE_SKIP_RPATH ON)
set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_LINKER} ${AC_LINKER_FLAGS} -o ${EXE_NAME}.elf <OBJECTS> <LINK_LIBRARIES>")

set(BUILD_SHARED_LIBS OFF)

add_subdirectory(src)


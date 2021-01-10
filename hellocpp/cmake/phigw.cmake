set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(MCU_LINKER_SCRIPT "${NUTTX_PATH}/scripts/ld.script")

set(COMPILER_PREFIX arm-none-eabi-)

# cmake-format: off
set(CMAKE_C_COMPILER    ${COMPILER_PREFIX}gcc)
set(CMAKE_CXX_COMPILER  ${COMPILER_PREFIX}g++)
set(CMAKE_AR            ${COMPILER_PREFIX}ar)
set(CMAKE_RANLIB        ${COMPILER_PREFIX}ranlib)
set(CMAKE_LINKER        ${COMPILER_PREFIX}ld)
set(CMAKE_ASM_COMPILER  ${COMPILER_PREFIX}gcc)
set(CMAKE_OBJCOPY       ${COMPILER_PREFIX}objcopy)
set(CMAKE_OBJDUMP       ${COMPILER_PREFIX}objdump)
set(CMAKE_SIZE          ${COMPILER_PREFIX}size)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(AC_HW_FLAGS         "-mcpu=cortex-m4 -mthumb -mfloat-abi=soft -isystem ${NUTTX_PATH}/include -pipe")

set(AC_LINKER_FLAGS     "--entry=__start -nostartfiles -nostdlib -nodefaultlibs -T${MCU_LINKER_SCRIPT}")

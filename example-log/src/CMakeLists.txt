set(HEADER_FILES
	Log.h
	json.hpp
)

set(SOURCE_FILES
	Log.cpp
)

link_directories(${EXE_NAME} ${NUTTX_PATH}/libs)

add_executable(${EXE_NAME} ${SOURCE_FILES} main.cpp ${HEADER_FILES})

add_custom_command(
	TARGET ${EXE_NAME}
	POST_BUILD
	COMMAND ${CMAKE_OBJCOPY} ARGS -S -O binary ${CMAKE_BINARY_DIR}/${EXE_NAME}.elf ${CMAKE_BINARY_DIR}/${EXE_NAME}.bin
)

target_link_libraries(${EXE_NAME} --start-group)

target_link_libraries(${EXE_NAME} sched)
target_link_libraries(${EXE_NAME} drivers)
target_link_libraries(${EXE_NAME} boards)
target_link_libraries(${EXE_NAME} c)
target_link_libraries(${EXE_NAME} mm)
target_link_libraries(${EXE_NAME} arch)
target_link_libraries(${EXE_NAME} xx)
target_link_libraries(${EXE_NAME} apps)
target_link_libraries(${EXE_NAME} fs)
target_link_libraries(${EXE_NAME} binfmt)
target_link_libraries(${EXE_NAME} board)
target_link_libraries(${EXE_NAME} gcc)
target_link_libraries(${EXE_NAME} supc++)
target_link_libraries(${EXE_NAME} net)


target_link_libraries(${EXE_NAME} --end-group)

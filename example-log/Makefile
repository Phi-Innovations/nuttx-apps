BUILD_DIR_BASE=build
DEBUG?=ON
BASE_PATH=$(PWD)

all:
	@echo "Targets:"
	@echo " default - builds for the board"
	@echo " run     - builds and load on target"

.PHONY: _build
_build:
	@if [ "$(BUILD_TYPE)" != "default" ]; then \
		cmake -S . -B $(BUILD_DIR) -G Ninja \
			  	-DPARAM_DEBUG=$(DEBUG); \
	else \
		cmake -S . -B $(BUILD_DIR) -G Ninja \
			  	-DPARAM_DEBUG=$(DEBUG); \
	fi
	@cmake --build $(BUILD_DIR)

clean:
	@rm -rf $(BUILD_DIR_BASE)*

default:
	make BUILD_TYPE=$@ BUILD_DIR=$(BUILD_DIR_BASE) _build; \

run: default
	JLinkExe -commanderScript jlink/hello.jlink

update:
	@rm -rf nuttx-export-10.0.1
	@unzip ~/nuttx_ws/nuttx/nuttx-export-10.0.1.zip
	@cp -r nuttx-export-10.0.1_/include/netutils nuttx-export-10.0.1/include/
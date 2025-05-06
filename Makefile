RAYLIB_H_DIR     ?= "/home/temax/dev/c/libs/raylib/build/raylib/include"
LIBRAYLIB_A_PATH ?= "/home/temax/dev/c/libs/raylib/build/raylib/libraylib.a"
SHELL_FILE       ?= "./shell.html"

BUILD_DIR        ?= "./build"
IN_FILE          ?= "./game.c"

web: build_dir
	emcc -o $(BUILD_DIR)/web/game.html $(IN_FILE) -Os -Wall					\
          $(LIBRAYLIB_A_PATH) -I. -I$(RAYLIB_H_DIR)							\
          -L. -L$(LIBRAYLIB_A_PATH) -sUSE_GLFW=3 -sASYNCIFY					\
          -sFORCE_FILESYSTEM -sEXPORTED_FUNCTIONS=_main,_malloc,_free		\
          -sEXPORTED_RUNTIME_METHODS=ccall,cwrap,UTF8ToString,stringToUTF8	\
          -DPLATFORM_WEB --shell-file $(SHELL_FILE)

linux: build_dir
	cc -o $(BUILD_DIR)/linux/gravity_balls_linux_x86_64 $(IN_FILE) -lraylib -lm -Wextra -Wall

build_dir:
	mkdir -p $(BUILD_DIR)/linux
	mkdir -p $(BUILD_DIR)/web

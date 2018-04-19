TEST_SRC1 = ./src/main.cpp ./src/ui_controller.cpp ./src/video_player.cpp ./src/uavv_wrapper.cpp \
			./src/xloader.c ./src/main_panel.cxx ./src/render_panel.cxx ./src/zoom_panel.cxx \
			./src/zoom_view.cxx ./src/klv_panel.cxx ./src/klv_view.cxx ./src/draw_controller.cpp ./src/draw_model.cpp

TEST_BIN1 = CVTool$(BIN_EXT)

SOURCE = $(TEST_SRC1)
BINARY = $(TEST_BIN1)


# Include common macros
include ./common.mak


# Specify local settings
ifeq ($(ARCH),win32)
    $(BINARY):  LDFLAGS += -static -static-libgcc -static-libstdc++
    $(BINARY):  LDLIBS += -liconv
    CFLAGS += -mwindows
endif

ifeq ($(ARCH),linux)
    $(BINARY):  LDFLAGS += -L//usr/lib/x86_64-linux-gnu
endif

$(BINARY):  LDLIBS += $(FLTK_LIBS)
$(BINARY):  LDLIBS += -lstdc++ -pthread -lm libuavv_video.dll

CFLAGS += -I$(LOCALDIR)/includes
CFLAGS += -I$(LOCALDIR)/fltk
CFLAGS += -m64
CFLAGS += -DUSE_OPENGL32

# Define rules for local objects
$(TEST_BIN1):  $(call GetObj,$(TEST_SRC1)) $(EXTRA_LIBS)
	 $(CXX) $(LDFLAGS) $(call GetObj,$(TEST_SRC1)) $(LDLIBS) -o $(TEST_BIN1)

main.o:  ./src/main.cpp
	 $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $^

xloader.o:  ./src/xloader.c
	 $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $^

ui_controller.o:  ./src/ui_controller.cpp
	 $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $^

video_player.o:  ./src/video_player.cpp
	 $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $^

uavv_wrapper.o:  ./src/uavv_wrapper.cpp
	 $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $^

main_panel.o:  ./src/main_panel.cxx
	 $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $^

render_panel.o:  ./src/render_panel.cxx
	 $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $^

zoom_panel.o:  ./src/zoom_panel.cxx
	 $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $^

zoom_view.o:  ./src/zoom_view.cxx
	 $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $^

klv_panel.o:  ./src/klv_panel.cxx
	 $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $^

klv_view.o:  ./src/klv_view.cxx
	 $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $^

draw_controller.o:  ./src/draw_controller.cpp
	 $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $^

draw_model.o:  ./src/draw_model.cpp
	 $(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $^

.PHONY: clear

clear:
	 $(RM) $(call GetObj,$(TEST_SRC1))
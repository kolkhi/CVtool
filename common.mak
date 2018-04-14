## Common Makefile macros


# Make sure SOURCE has been defined
ifndef SOURCE
    ifndef TEST_SOURCE
        $(error Neither SOURCE nor TEST_SOURCE is defined)
    endif
endif

# Make sure BINARY has been defined
ifndef BINARY
    ifndef TEST_BINARY
        $(error Neither BINARY nor TEST_BINARY is defined)
    endif
endif

# Determine TOPDIR
ifndef TOPDIR
    COMMON_MAK := $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
	PARENT_DIR := $(patsubst %common.mak,%,$(COMMON_MAK))
	ifeq ($(PARENT_DIR),)
        PARENT_DIR := ./
    endif
    TOPDIR := $(abspath $(PARENT_DIR))
    #$(info $(COMMON_MAK))
    #$(info $(PARENT_DIR))
    #$(info $(TOPDIR))
    ifeq ($(TOPDIR),$(COMMON_MAK))
        TOPDIR := $(abspath .)
    endif
endif

# Determine system name
include $(TOPDIR)/arch.mak

# Setup common options
AR    = $(CROSS_COMPILE)ar
AS    = $(CROSS_COMPILE)as
CC    = $(CROSS_COMPILE)gcc
CXX   = $(CROSS_COMPILE)g++
LD    = $(CROSS_COMPILE)ld
STRIP = $(CROSS_COMPILE)strip
FL    = $(LOCALDIR)/fltk/fluid
PDFLATEX = pdflatex

ifdef HEADLESS
    CFLAGS += -DHEADLESS
endif

ifdef NO_OPTIMISATION
    CFLAGS += -O0
else
    CFLAGS += -O3
endif

ifdef DEBUG
    CFLAGS  += -g
    NO_STRIP = 1
else
    ifdef SANITIZE
        CFLAGS  += -g -fsanitize=address -fno-omit-frame-pointer
        LDLIBS  += -lasan
        NO_STRIP = 1
    else
        CFLAGS += -funroll-loops -ftree-vectorize
        ifneq ($(ARCH),mac)
            CFLAGS += -fopt-info-vec
        endif
    endif
endif

ifdef PROFILE
    CFLAGS  += -pg -fno-omit-frame-pointer
    LDFLAGS += -pg
    NO_STRIP = 1
endif

CFLAGS += -Wall -Wno-unused
CFLAGS += -I$(TOPDIR)/core
CFLAGS += -D_FILE_OFFSET_BITS=64
CFLAGS += -D__USE_FILE_OFFSET64
CFLAGS += -D__USE_LARGEFILE64
CFLAGS += -D_LARGEFILE64_SOURCE


# Setup options base on system name
ifeq ($(ARCH),win32)
    ifeq ($(MACHINE),x64)
        LOCALDIR = $(TOPDIR)
    endif
endif
ifeq ($(ARCH),mac)
    LOCALDIR = $(TOPDIR)
endif
ifeq ($(ARCH),linux)
    ifeq ($(MACHINE),x64)
        LOCALDIR = $(TOPDIR)
    endif
    ifeq ($(MACHINE),armv7l)
        LOCALDIR = $(TOPDIR)/local/armv7l
    endif
endif

ifndef EXCLUDE_LOCAL_DIR
    ifdef LOCALDIR
        CFLAGS  += $(call GetCFlags,$(LOCALDIR)/includes)
        LDFLAGS += $(call GetLdFlags,$(LOCALDIR)/fltk/lib)
    endif
endif

LICENSE_LIBS   = -ltomcrypt
FFMPEG_LIBS    = -lswscale -lavformat -lavdevice -lavcodec -lavutil -lopenh264
ifndef HEADLESS
FLTK_LIBS      = -lfltk_gl -lfltk
endif
ifeq ($(ARCH),mac)
PNG_LIBS       = /opt/local/lib/libpng.a /opt/local/lib/libz.a
else
PNG_LIBS       = -lpng -lz
endif
AAPAINTER_LIBS = -lagg
MAPENGINE_LIBS = -lminizip -lz -lcartotype -liksemel $(AAPAINTER_LIBS)
JSON_LIBS      = -lcjson

ifeq ($(ARCH),win32)
    # MinGW
    CFLAGS  += -DWIN32
    ifeq ($(MACHINE),x64)
        CFLAGS  += -DWIN64
    endif
    CFLAGS  += -D__STDC_CONSTANT_MACROS
    CFLAGS  += -D_USE_MATH_DEFINES
    CFLAGS  += -D_GLIBCXX_INCLUDE_NEXT_C_HEADERS
    CFLAGS  += -D__USE_MINGW_ANSI_STDIO
    BIN_EXT       := .exe
    DSO_EXT       := .dll
    INSTALLER_EXT := .exe
    NETWORK_LIBS = -lwsock32 -lws2_32
    ifndef HEADLESS
        FLTK_LIBS    += -lglu32 -lopengl32 -lole32 -luuid -lcomctl32
        FLTK_LIBS    += -lgdi32 -lcomdlg32 -lshell32
    endif
    INTL_LIBS    = -lintl
    JOYSTICK_LIBS = -lwinmm
    FFMPEG_LIBS   += -lsecur32 $(NETWORK_LIBS) -lole32
else
    # Other POSIX systems
    ifeq ($(ARCH),mac)
        DSO_EXT       := .dylib
        INSTALLER_EXT := .dmg
        CFLAGS  += -DMAC
        CFLAGS  += -I/opt/local/include
        LDFLAGS += -L/opt/local/lib
        INTL_LIBS     = /opt/local/lib/libintl.a /opt/local/lib/libiconv.a
        NETWORK_LIBS  = -framework IOKit -framework CoreFoundation
        JOYSTICK_LIBS = -framework IOKit -framework CoreFoundation
        LICENSE_LIBS  += -framework IOKit -framework CoreFoundation
        FFMPEG_LIBS   += -framework VideoDecodeAcceleration
        FFMPEG_LIBS   += -framework VideoToolbox -framework CoreVideo
        FFMPEG_LIBS   += -framework AVFoundation -framework AudioToolbox
        FFMPEG_LIBS   += -framework CoreMedia -framework QuartzCore
        FFMPEG_LIBS   += -framework Security -framework CoreFoundation
        ifndef HEADLESS
            FLTK_LIBS     += -framework OpenGL -framework Cocoa
        endif
    else
        DSO_EXT := .so
        CFLAGS  += -DLINUX
        LDLIBS  += -lrt
        ifeq ($(MACHINE),imx6)
            CFLAGS  += -DIMX6
            CFLAGS  += -I/opt/fsl/include
            LDFLAGS += -L/opt/fsl/lib
            IMXVPU_LIBS = -limxvpuapi
        endif
        ifeq ($(MACHINE),x64)
            FFMPEG_LIBS += -lvdpau -lX11
        endif
        NETWORK_LIBS  =
        JOYSTICK_LIBS =
        FFMPEG_LIBS   += -ldl
        ifndef HEADLESS
            FLTK_LIBS     += -lGLU -lGL -lXft -lfontconfig
            FLTK_LIBS     += -lXinerama -lXrender -lXcursor -lXfixes -lXext
            FLTK_LIBS     += -lX11 -lpthread -ldl -lm
        endif
        ifdef DC1394
	    CFLAGS        += -DUSE_DC1394
            DC1394_LIBS   += -ldc1394 -lraw1394
        endif
    endif
    COMPILE.c += -std=c99
    CFLAGS    += -fPIC
    ifneq ($(ARCH),mac)
    endif
    XLOADER_LIBS  += -ldl
endif

# Setup extra directories
vpath %.h $(EXTRA_DIRS)
CFLAGS  += $(call GetCFlags,$(EXTRA_DIRS))
LDFLAGS += $(call GetLdFlags,$(EXTRA_DIRS))

# Let C++ compiler use the same option flags as C
CXXFLAGS = $(CFLAGS)
ifeq ($(CXX),g++)
    CXXFLAGS += -Wno-non-virtual-dtor
endif
ifeq ($(ARCH),mac)
    CXXFLAGS += -std=c++0x
endif

# Setup uuidgen options
UUIDGEN = uuidgen

# Setup archiver options
ARFLAGS = rcs

# Setup strip options
ifeq ($(ARCH),mac)
    STRIPFLAGS = -S -x
else
    STRIPFLAGS = --strip-unneeded --discard-locals
endif

# Setup macros for automatically generated targets
SRCTYPES = c cc cpp cxx fl rc
GetObj = $(notdir $(foreach s,$(SRCTYPES),$(patsubst %.$(s),%.o,$(filter %.$(s),$1))))

# List automatically generated items
FL_HDR_EXT ?= h
FL_SRC_EXT ?= cpp
FL_SOURCE = $(filter %.fl,$(SOURCE) $(TEST_SOURCE))
GENERATED_HDR += $(FL_SOURCE:fl=$(FL_HDR_EXT))
GENERATED_SRC += $(FL_SOURCE:fl=$(FL_SRC_EXT))
GENERATED_OBJ += $(call GetObj,$(SOURCE) $(TEST_SOURCE))

#$(info $(GENERATED_OBJ))
#$(info $(SOURCE))
#tt=$(call GetObj,$(SOURCE))#$(info $(tt))
# Setup rules for executables
%: %.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
ifndef NO_STRIP
	$(STRIP) $(STRIPFLAGS) $@
	$(call SignFile,$@)
endif

# Setup rules for EXE files
.SUFFIXES: .exe
.o.exe:
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@
	chmod +x $@
ifndef NO_STRIP
	$(STRIP) $(STRIPFLAGS) $@
	$(call SignFile,$@)
endif

# Setup rules for SO files
.SUFFIXES: .so
.o.so:
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -shared -fPIC -o $@
ifndef NO_STRIP
	$(STRIP) $(STRIPFLAGS) $@
	$(call SignFile,$@)
endif

# Setup rules for DLL files
.SUFFIXES: .dll
.o.dll:
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -shared -o $@
ifndef NO_STRIP
	$(STRIP) $(STRIPFLAGS) $@
	$(call SignFile,$@)
endif

# Setup rules for FLTK layouts
.SUFFIXES: .fl
.fl.$(FL_HDR_EXT) .fl.$(FL_SRC_EXT):
	$(FL) $(FLFLAGS) -c $<

# Setup rules for Windows resource files
.SUFFIXES: .rc
RCFLAGS += -c 65001
.rc.o:
	windres $(RCFLAGS) -o $@ $<

# Setup rules for generating MacOS EULA resource file
.SUFFIXES: .rsrc .txt
.txt.rsrc:
	$(LOCALDIR)/bin/eula2rsrc <$< >$@

# Setup rules for code signing
ifneq ($(wildcard $(TOPDIR)/signing/signtool.mak),)
    include $(TOPDIR)/signing/signtool.mak
    ifeq ($(ARCH),win32)
        SignFile = $(TOPDIR)/signing/signtool.exe sign \
            -f $(TOPDIR)/signing/$(signtool_cert) \
            -p $(signtool_passwd) \
            -t $(signtool_timeurl) $1
    endif
    ifeq ($(ARCH),mac)
        SignFile = codesign --force --signature-size=12800 \
            --sign "$(signtool_identity)" --timestamp $1
    endif
endif

# Setup rules for building NSIS installers
ifeq ($(ARCH),$(filter msvc win32,$(ARCH)))
    MAKENSIS = makensis
    NSISFLAGS += -DTOPDIR="$(call NativePath,$(TOPDIR))"
    NSISFLAGS += -DTOPDIR_INC="$(call NativePath,$(TOPDIR)/local/nsis)"
endif
.SUFFIXES: .nsi
BuildNSI = $(MAKENSIS) $(NSISFLAGS) $1

# Setup rules for building DMG files
.SUFFIXES: .dmg
BuildDMG = \
    hdiutil create -ov -srcfolder $2 -volname $1 $1; \
    hdiutil unflatten $1; \
    Rez -a eula.rsrc -o $1; \
    hdiutil flatten $1;

# Setup rules for PDF files
.SUFFIXES: .tex .pdf
.tex.pdf:
	# Typeset twice to make sure references are correctly updated
	$(PDFLATEX) -interaction=batchmode $< && \
	$(PDFLATEX) -interaction=batchmode $< && \
	(grep '^LaTeX Warning' $*.log || :)

# Define common targets
all:  binary

# Include rules for gettext translation
include $(TOPDIR)/gettext.mak

binary:  $(BINARY) $(GETTEXT_OBJ)

test:  $(TEST_BINARY)

uuid:
	$(UUIDGEN)

clean:
	$(RM) $(GENERATED_HDR) $(GENERATED_SRC) $(GENERATED_OBJ)
	$(CLEAN_EXTRA)

distclean:  clean
	$(RM) $(BINARY) $(TEST_BINARY) gmon.out
ifneq ($(GETTEXT_OBJ),)
	$(RM) $(GETTEXT_OBJ)
	rmdir -p $(patsubst %,$(GETTEXT_DESTDIR)/%/LC_MESSAGES,$(GETTEXT_LANG)) ||:
endif
	$(DISTCLEAN_EXTRA)

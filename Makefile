BREAKPAD_SRCDIR := ../breakpad/src
BREAKPAD_OBJDIR := ../obj-breakpad

OS := $(strip $(shell uname -s))

SOURCES := test_crash.cpp

ifeq ($(OS),Linux)
BREAKPAD_LIBS := \
  $(BREAKPAD_OBJDIR)/src/client/linux/libbreakpad_client.a
LIBS := -lpthread
endif

ifeq ($(OS),Darwin)
MAC_SOURCES := \
  src/client/minidump_file_writer.cc \
  src/client/mac/handler/breakpad_nlist_64.cc \
  src/client/mac/handler/dynamic_images.cc \
  src/client/mac/handler/exception_handler.cc \
  src/client/mac/handler/minidump_generator.cc \
  src/client/mac/handler/protected_memory_allocator.cc \
  src/client/mac/crash_generation/crash_generation_client.cc \
  src/client/mac/crash_generation/crash_generation_server.cc \
  src/common/convert_UTF.c \
  src/common/md5.cc \
  src/common/string_conversion.cc \
  src/common/mac/MachIPC.mm \
  src/common/mac/arch_utilities.cc \
  src/common/mac/bootstrap_compat.cc \
  src/common/mac/file_id.cc \
  src/common/mac/macho_id.cc \
  src/common/mac/macho_reader.cc \
  src/common/mac/macho_utilities.cc \
  src/common/mac/macho_walker.cc \
  src/common/mac/string_utilities.cc \
  $(NULL)

SOURCES += \
  $(addprefix $(BREAKPAD_SRCDIR)/,$(MAC_SOURCES))

LIBS := -framework CoreFoundation
endif

ifeq (MINGW32,$(OS:MINGW32%=MINGW32))
EXE := .exe
OBJ_SUFFIX := .obj
LIBS := \
  $(BREAKPAD_SRCDIR)/src/client/windows/Release/lib/common.lib \
  $(BREAKPAD_SRCDIR)/src/client/windows/Release/lib/crash_generation_client.lib \
  $(BREAKPAD_SRCDIR)/src/client/windows/Release/lib/crash_generation_server.lib \
  $(BREAKPAD_SRCDIR)/src/client/windows/Release/lib/exception_handler.lib
CXX := cl
COMPILE := $(CXX) -c -Fo
LINK := link -MACHINE:X64 -OUT:
else
EXE :=
OBJ_SUFFIX := .o
CXXFLAGS += -std=c++11
COMPILE := $(CXX) -c -o 
LINK := $(CXX) -o 
endif

CXXFLAGS += -I$(BREAKPAD_SRCDIR)/src

OBJS := $(addsuffix $(OBJ_SUFFIX),$(basename $(notdir $(SOURCES))))

test_crash$(EXE): $(OBJS)
	$(LINK)$@ $^ $(BREAKPAD_LIBS) $(LIBS)

define srcdep
$(basename $(notdir $1))$(OBJ_SUFFIX): $1
endef
$(foreach f,$(SOURCES),$(eval $(call srcdep,$(f))))

$(OBJS):
	$(COMPILE)$@ $(CXXFLAGS) $<

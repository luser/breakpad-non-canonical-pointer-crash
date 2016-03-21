BREAKPAD_SRCDIR := ../breakpad/src
BREAKPAD_OBJDIR := ../obj-breakpad

OS := $(shell uname -s)

ifeq ($(OS),Linux)
BREAKPAD_LIBS := \
  $(BREAKPAD_OBJDIR)/src/client/linux/libbreakpad_client.a
LIBS := -lpthread
endif

CXXFLAGS := -I$(BREAKPAD_SRCDIR)/src

test_crash: test_crash.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(BREAKPAD_LIBS) $(LIBS)

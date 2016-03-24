#include <cstdio>
#include <stdlib.h>

#ifdef __linux__
#include "client/linux/handler/exception_handler.h"
using google_breakpad::MinidumpDescriptor;
#endif
#ifdef __APPLE__
#include "client/mac/handler/exception_handler.h"
#endif
#ifdef _WIN32
#include "client/windows/handler/exception_handler.h"
#endif


// Callback when minidump written.
static bool MinidumpCallback(
#if defined(__linux__)
                             const MinidumpDescriptor& md,
#elif defined(__APPLE__)
                             const char* dir,
                             const char* minidump_id,
#elif defined(_WIN32)
                             const wchar_t* dir,
                             const wchar_t* minidump_id,
#endif
                             void *context,
#if defined(_WIN32)
                             EXCEPTION_POINTERS* exinfo,
                             MDRawAssertionInfo* assertion,
#endif

                             bool succeeded) {
#ifdef _WIN32
    printf("%S is dumped\n", minidump_id);
#else
    printf("%s is dumped\n",
#ifdef __linux__
    md.path()
#else
      minidump_id
#endif
      );
#endif
  _exit(1);
  return succeeded;
}

void do_crash(volatile int* address) {
  *address = 1;
}

typedef void (*Func)();

int main(int argc, char *argv[]) {
#if defined(__linux__)
  MinidumpDescriptor descriptor(".");
  google_breakpad::ExceptionHandler handler_process(descriptor, NULL, MinidumpCallback,
                                   NULL, true, -1);
#elif defined(__APPLE__)
  google_breakpad::ExceptionHandler handler_process(".", NULL, MinidumpCallback,
                                   NULL, true, NULL);
#elif defined(_WIN32)
  google_breakpad::ExceptionHandler handler_process(L".",
                                                    NULL,
                                                    MinidumpCallback,
                                                    NULL,
                                                    google_breakpad::ExceptionHandler::HANDLER_ALL);
#endif

  // Non-canonical pointer.
  const uintptr_t kBadAddress = 0x7ffffffff0dea000;
  if (argc == 1) {
    int* foo = reinterpret_cast<int*>(kBadAddress);
    do_crash(foo);
  } else {
    volatile Func f = reinterpret_cast<Func>(kBadAddress);
    f();
  }

  return 0;
}

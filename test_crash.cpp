#include <cstdio>

#ifdef __linux__
#include "client/linux/handler/exception_handler.h"
using google_breakpad::MinidumpDescriptor;
#endif
#ifdef __APPLE__
#include "client/mac/handler/exception_handler.h"
#endif

// Callback when minidump written.
static bool MinidumpCallback(
#ifdef __linux__
                             const MinidumpDescriptor& md,
#else
                             const char* dir,
                             const char* minidump_id,
#endif
                             void *context,
                             bool succeeded) {
    printf("%s is dumped\n",
#ifdef __linux__
    md.path()
#else
      minidump_id
#endif
      );
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
#endif

  // Non-canonical pointer.
  const uintptr_t kBadAddress = 0x7ffffffff0dea000;
  if (argc == 1) {
    int* foo = reinterpret_cast<int*>(kBadAddress);
    do_crash(foo);
  } else {
    Func f = reinterpret_cast<Func>(kBadAddress);
    f();
  }

  return 0;
}

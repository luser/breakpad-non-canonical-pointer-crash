#include <cstdio>

#ifdef __linux__
#include <client/linux/handler/exception_handler.h>
#endif

using namespace google_breakpad;

// Callback when minidump written.
static bool MinidumpCallback(const MinidumpDescriptor& md,
                             void *context,
                             bool succeeded) {
  printf("%s is dumped\n", md.path());
  _exit(1);
  return succeeded;
}

void do_crash(volatile int* address) {
  *address = 1;
}

typedef void (*Func)();

int main(int argc, char *argv[]) {
  MinidumpDescriptor descriptor(".");
  ExceptionHandler handler_process(descriptor, NULL, MinidumpCallback,
                                   NULL, true, -1);

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

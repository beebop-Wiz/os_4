#include <asmintr.h>
void test();

void kernel_main() {
  test();
  // force a reset to halt QEMU
  unsigned char good = 0x02;
  while (good & 0x02)
    good = inb(0x64);
  outb(0x64, 0xFE);
}

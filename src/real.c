#include "real.h"

void bios_intr(int errupt) {
  rmregs.int_no = errupt;
  pmode_to_real();
}

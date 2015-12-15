#include "keyboard.h"
#include "pic.h"
#include "idt.h"
#include "vgatext.h"
#include "port.h"
#include "malloc.h"

unsigned int kbdus[2][128] = {
  {
    0, '\e', '1', '2', '3', '4', '5', '6',
    '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
    'o', 'p', '[', ']', '\n', BUCKY_CTL, 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
    '\'', '`', BUCKY_SHIFT, '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', BUCKY_SHIFT, 0,
    BUCKY_ALT, ' ', BUCKY_META
  }, {
    0, '\e', '!', '@', '#', '$', '%', '^',
    '&', '*', '(', ')', '_', '+', '\b', '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
    'O', 'P', '{', '}', '\n', BUCKY_CTL, 'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
    '"', '~', BUCKY_SHIFT, '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', '?', BUCKY_SHIFT, 0,
    BUCKY_ALT, ' ', BUCKY_META
  }    
};

volatile int kbd_state = 0;

void setup_keyboard() {
  PIC_clear_line(1);
  register_irq_handler(1, keyboard_intr);
  kbdbuf = malloc(KBD_BUFSIZ);
}

void keyboard_intr() {
  unsigned char scan = inb(0x60);
  kbdbuf[kbwritep++] = scan;
  if(kbwritep > KBD_BUFSIZ) kbwritep = 0;
  if(scan < 0x80) {
    if(kbdus[kbd_state % 2][(int) scan] > (int) 0xff) {
      kbd_state |= kbdus[kbd_state % 2][(int) scan] >> 8;
    } else {
      printf("%c", kbdus[kbd_state % 2][(int) scan]);
      vga_redraw();
    }
  } else if(kbdus[kbd_state % 2][(int) (scan & 0x7f)] > 0xff) {
    kbd_state &= ~kbdus[kbd_state % 2][(int) (scan & 0x7f)] >> 8;
  }
}

char get_next_sc() {
  if(kbreadp > KBD_BUFSIZ) kbreadp = 0;
  return (kbwritep != kbreadp) ? kbdbuf[kbreadp++] : -1;
}

int keyboard_getchar() {
  char scan;
  do {
    scan = get_next_sc();
  } while(scan & 0x80 && scan != -1);
  if(scan == -1) return -1;
  return kbdus[kbd_state % 2][(int) scan];
}

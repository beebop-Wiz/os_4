#include "keyboard.h"
#include "pic.h"
#include "idt.h"
#include "vgatext.h"
#include "port.h"
#include "malloc.h"
#include "mt.h"

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
unsigned char *kbdbuf;
volatile int kbwritep = 0, kbreadp = 0;

void setup_keyboard() {
  PIC_clear_line(1);
  register_irq_handler(1, keyboard_intr);
  kbdbuf = malloc(KBD_BUFSIZ);
}

void keyboard_intr() {
  unsigned char scan = inb(0x60);
  if(scan < 0x80) {
    if(kbdus[kbd_state % 2][(int) scan] > (int) 0xff) {
      kbd_state |= kbdus[kbd_state % 2][(int) scan] >> 8;
    } else if(kbd_state & (BUCKY_CTL >> 8)) {
      char c = kbdus[kbd_state % 2][(int) scan];
      printf("^%c", kbdus[kbd_state % 2][(int) scan]);
      if(c == 'c') {		/* SIGINT */
	signal_foreground(9);
      } else if(c == 'd') {	/* EOF */
	kbdbuf[kbwritep++] = 255;
	if(kbwritep > KBD_BUFSIZ) kbwritep = 0;
      } else if(c == 'z') {	/* SIGSTOP */
	signal_foreground(14);
      }
    } else {
      printf("%c", kbdus[kbd_state % 2][(int) scan]);
      //      vga_redraw();
      kbdbuf[kbwritep++] = kbdus[kbd_state % 2][(int) scan];
      if(kbwritep > KBD_BUFSIZ) kbwritep = 0;
    }
  } else if(kbdus[kbd_state % 2][(int) (scan & 0x7f)] > 0xff) {
    kbd_state &= ~kbdus[kbd_state % 2][(int) (scan & 0x7f)] >> 8;
  }
}

#ifndef KEYBOARD_H
#define KEYBOARD_H

#define KBD_BUFSIZ 1024

void setup_keyboard();
void keyboard_intr();
int keyboard_getchar();

#define KBD_CHAR(sc) ((char) (sc) & 0x7f)
#define KBD_BUCKY(sc) ((sc) & 0xffffff00)

#define BUCKY_SHIFT 0x0100
#define BUCKY_ALT   0x0200
#define BUCKY_CTL   0x0400
#define BUCKY_META  0x0800
#define BUCKY_SUPER 0x1000
#define BUCKY_HYPER 0x2000

#endif

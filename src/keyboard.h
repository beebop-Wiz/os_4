#ifndef KEYBOARD_H
#define KEYBOARD_H

char *volatile kbdbuf;
volatile int kbwritep, kbreadp;

#define KBD_BUFSIZ 1024

void setup_keyboard();
void keyboard_intr();
char keyboard_getchar();

#endif

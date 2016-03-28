#ifndef USER_H
#define USER_H

void jump_usermode();
void tss_flush();
void call_usermode(struct registers *r, int id, int data);
#endif

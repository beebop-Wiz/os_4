void ring3_main() {
  char *foo = "foo\n";
  asm ("mov $0, %%eax\nmov %0, %%ebx\nint $0x81" : : "m" (foo) : "eax", "ebx");
  // no way to return to kcode right now
  for(;;);
}

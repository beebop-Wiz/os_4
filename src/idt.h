#ifndef IDT_H
#define IDT_H

struct idt_gate {
  unsigned short offset_lo;
  unsigned short selector;
  unsigned char res;
  unsigned char type;
  unsigned short offset_hi;
} __attribute__ ((packed));

typedef struct idt_gate idt_gate_t;

struct idtr {
  unsigned short size;
  unsigned int offset;
} __attribute__ ((packed));

typedef struct idtr idtr_t;

#define TYPE_PRESENT 0x80
#define TYPE_DPL(i)  (((i) & 0x3) << 5)
#define TYPE_STORAGE 0x10
#define TYPE_32TASK  0x5
#define TYPE_16INT   0x6
#define TYPE_16TRAP  0x7
#define TYPE_32INT   0xE
#define TYPE_32TRAP  0xF

// for convenience
#define TYPE_USR_32I TYPE_PRESENT | TYPE_DPL(3) | TYPE_32INT
#define TYPE_SYS_32I TYPE_PRESENT | TYPE_DPL(0) | TYPE_32INT
#define MAKE_ADDR(s, a) do { s.offset_lo = ((unsigned int) a) & 0xFFFF; s.offset_hi = (((unsigned int) a) >> 16) & 0xFFFF;} while(0);

struct registers {
  unsigned short gs, fs, es, ds;                       // 0 2 4 6
  unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax; // 8 12 16 20 24 28 32 36
  unsigned int int_no, err;			       // 40 44
  unsigned int eip, cs, eflags, useresp, ss;           // 48 52 56 60 64
} __attribute__ ((packed));

typedef struct registers * regs_t;

/**
 * Loads the provided IDTR with the ASM `lidt` instruction.
 * @param[in] idt The address of the IDTR struture. Note that this is
 * NOT the address of the IDT itself.
 */
void load_idt(idtr_t *idt);

/**
 * Sets up the IDT with all defined interrupt handlers.
 */
void setup_idt();
#endif

#ifndef VBE_H
#define VBE_H

struct vbe_info {
  char magic[4];
  unsigned short version;
  unsigned short ostr[2];
  unsigned char caps[4];
  unsigned short vmptr[2];
  unsigned short total_mem;
} __attribute__ ((packed));

struct vbe_mode {
  unsigned short attrs;
  unsigned char winA, winB;
  unsigned short gran;
  unsigned short win;
  unsigned short segA, segB;
  unsigned short wpf[2];
  unsigned short pitch;

  unsigned short xres, yres;
  unsigned char wchar, ychar, planes, bpp, banks;
  unsigned char model, banksize, pages;
  unsigned char res0;

  unsigned char rm, rp;
  unsigned char gm, gp;
  unsigned char bm, bp;
  unsigned char xm, xp;
  unsigned char dc_attrs;

  unsigned int lfb_base;
} __attribute__ ((packed));

/**
 * Loads VBE information.
 */
void vbe_load_data();

#endif

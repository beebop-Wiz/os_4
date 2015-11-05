#ifndef VGA_H
#define VGA_H

#define N_SEQ_IDX 5
#define N_CRTC_IDX 19
#define N_GC_IDX 9
#define N_ATTR_IDX 15

// ports
#define VGA_ATTR    0x3C0 /**< VGA index/data port. Alternates between
			      index and data bytes. Read VGA_IDX_RST
			      to reset it to the 'index' state. */
#define VGA_IDX_RST 0x3DA
#define VGA_MOR     0x3C2 /**< VGA misc. output register. */
#define VGA_MIR     0x3CC /**< VGA misc. input register. */
#define VGA_SEQ     0x3C4
#define VGA_GC      0x3CE
#define VGA_CRTC    0x3D4
#define VGA_DACMASK 0x3C6
#define VGA_DAC1    0x3C8
#define VGA_DAC2    0x3C9
#define VGA_DAC3    0x3C7

// indices
#define ATTR_MCTL   0x10
#define ATTR_OSCN   0x11
#define ATTR_CPE    0x12
#define ATTR_HPAN   0x13
#define ATTR_CSEL   0x14

#define SEQ_RST     0x00
#define SEQ_CMODE   0x01
#define SEQ_MMASK   0x02
#define SEQ_CSEL    0x03
#define SEQ_MMODE   0x04

#define GC_SET_RST  0x00
#define GC_EN_ST_RT 0x01
#define GC_COMPARE  0x02
#define GC_ROTATE   0x03
#define GC_MODE     0x05
#define GC_MISC     0x06
#define GC_BITMASK  0x08

#define CRTC_HTOTAL 0x00
#define CRTC_HDEE   0x01
#define CRTC_HBLS   0x02
#define CRTC_HBLE   0x03
#define CRTC_HRTS   0x04
#define CRTC_HRTE   0x05
#define CRTC_VTOTAL 0x06
#define CRTC_OFREG  0x07
#define CRTC_PRSCAN 0x08
#define CRTC_MAXSCN 0x09
#define CRTC_VRTS   0x10
#define CRTC_VRTE   0x11
#define CRTC_VDEE   0x12
#define CRTC_LWIDTH 0x13
#define CRTC_UNDLOC 0x14
#define CRTC_VBLS   0x15
#define CRTC_VBLE   0x16
#define CRTC_MCTL   0x17

// flags
#define MOR_IOAS    0x01
#define MOR_RAMEN   0x02
#define MOR_CLOCK(i) (((i) & 0x3) << 2)
#define MOR_OEPAGE  0x20
#define MOR_HSYNCP  0x40
#define MOR_VSYNCP  0x80

#define MCTL_ATGE   0x01
#define MCTL_MONO   0x02
#define MCTL_LGE    0x04
#define MCTL_BLINK  0x08
#define MCTL_PPM    0x20
#define MCTL_8BIT   0x40
#define MCTL_P54S   0x80

#define RST_ASYNC   0x01
#define RST_SYNC    0x02

#define CMODE_98DM  0x01
#define CMODE_SLR   0x04
#define CMODE_DCR   0x08
#define CMODE_S4    0x10
#define CMODE_SD    0x20

#define CSEL_A(i) ((((i) & 0x03) << 2) | (((i) & 0x04) << 3))
#define CSEL_B(i) (((i) & 0x03) | (((i) & 0x04) << 2))

#define MMODE_EXTM 0x02
#define MMODE_OE   0x04
#define MMODE_CH4  0x08

#define MODE_WRITE(i) ((i) & 0x03)
#define MODE_READ(i) ((i) << 3)
#define MODE_HOE   0x10
#define MODE_SHR   0x20
#define MODE_SH256 0x40

#define MISC_ADIS  0x01
#define MISC_CHOE  0x02
#define MISC_MMAP(i) (((i) & 0x03) << 2)

#define ROTATE_COUNT(i) ((i) & 0x03)
#define ROTATE_IDENT 0x00
#define ROTATE_AND   0x08
#define ROTATE_OR    0x10
#define ROTATE_XOR   0x18

  
/**
 * Initialize the VGA subsystem and clear the screen. Currently
 * initializes it in graphics mode.
 */
void init_vga(void);

/**
 * Writes one byte to an indexed VGA register.
 * @param[in] port The base port of the register.
 * @param[in] idx  The index of the VGA register.
 * @param[in] dat  The data to write.
 */
void vga_write_reg(unsigned short port, unsigned char idx, unsigned char dat);

/**
 * Draws one pixel.
 * @param[in] x X coordinate of the pixel.
 * @param[in] y Y coordinate of the pixel.
 * @param[in] color Color of the pixel.
 */
void vga_write_pix(int x, int y, int color);


/**
 * Sets one palette entry.
 * @param[in] idx Color index (1-256)
 * @param[in] r   Red component (0-63)
 * @param[in] g   Green component (0-63)
 * @param[in] b   Blue component (0-63)
 */
void vga_set_palette(int idx, unsigned char r, unsigned char g, unsigned char b);

/**
 * Resets the screen to color 0.
 */
void vga_clear();

int vga_get_color(int x, int y);
#endif

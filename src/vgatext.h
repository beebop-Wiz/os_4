#ifndef VGATEXT_H
#define VGATEXT_H

#define vga_itoa(i) vga_itoa_u(i, 10)

/**
 * Sets up the VGA text rendering.
 * Resets the cursor position and color info.
 */
void init_vgatext(void);

/**
 * Prints one character. Specials (\n, \t) work.
 * Redraws the screen on a \n.
 * @param[in] c The character to print.
 */
void vga_putchar(int c);

/**
 * Redraws the display from the terminal data.
 */
void vga_redraw();

/**
 * Clears the display.
 */

void vga_clear_text();


/**
 * Sets the current text color.
 */

void vga_set_color(int fg, int bg);

/**
 * Scrolls the display by one line. Does NOT erase the bottom line at
 * present.
 */
void vga_scroll();

/**
 * Sets the VGA window size and offset.
 */

void vga_setwin(int w, int h, int x, int y);

/**
 * you should know what printf does.
 * @param[in] fmt Format string
 */
void printf(const char *fmt, ...);

void vprintf(const char *fmp, __builtin_va_list ap);
void vga_addch(int x, int y, int c);

void vga_update_curs();
void vga_statchar(int c, int x);
#endif

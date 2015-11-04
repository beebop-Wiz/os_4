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
void vga_putchar(char c);

/**
 * Writes a null-terminated string.
 * @param[in] s The string to print.
 */
void vga_puts(char *s);

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

#endif

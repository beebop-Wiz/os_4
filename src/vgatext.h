#ifndef VGATEXT_H
#define VGATEXT_H

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
 * Scrolls the display by one line. Does NOT erase the bottom line at
 * present.
 */
void vga_scroll();

#endif

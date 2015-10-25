#ifndef PORT_H
#define PORT_H

/**
 * Writes one byte to a port.
 * @param[in] port The port to write to.
 * @param[in] val  The byte to emit.
 */
void outb(unsigned short port, unsigned char val);

/**
 * Writes one word (2 bytes) to a port.
 * @param[in] port The port to write to.
 * @param[in] val  The word to emit.
 */
void outw(unsigned short port, unsigned short val);

/**
 * Reads one byte from a port.
 * @param[in] port The port to read from.
 * @return The byte read from the port.
 */
unsigned char inb(unsigned short port);

/**
 * Reads one word from a port.
 * @param[in] port The port to read from.
 * @return The word read from the port.
 */
unsigned short inw(unsigned short port);
#endif

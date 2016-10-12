#ifndef DISK_H
#define DISK_H

void read_sector(unsigned int lba, unsigned char *mem);
void write_sector(unsigned int lba, unsigned char *mem);

#endif

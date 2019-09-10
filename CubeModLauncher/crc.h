#ifndef CRC_H
#define CRC_H

unsigned int crc32_buf(const char* buf, unsigned long len);
unsigned int crc32_file(const char* fileName);

#endif // CRC_H

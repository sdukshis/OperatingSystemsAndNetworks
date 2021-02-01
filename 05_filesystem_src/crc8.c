/* Compute CRC8 code of file contents */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

const int BUFF_SIZE = 256;

unsigned char crc8(unsigned char *pcBlock, unsigned int len, unsigned char init);

int filecrc8(int fd); /* read all file contents and return it crc8 */

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filepath>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }
    int filecrc = filecrc8(fd);
    if (filecrc == -1) {
        perror("filecrc8");
        close(fd);
        return EXIT_FAILURE;
    }
    printf("%x\n", filecrc);
    close(fd);
    return EXIT_SUCCESS;
}

int filecrc8(int fd) {
    int crc = 0xFF;  /* CRC-8 init */
    ssize_t rb = 0;
    unsigned char buff[BUFF_SIZE];
    while ((rb = read(fd, buff, BUFF_SIZE)) > 0) {
        crc = crc8(buff, rb, crc);
    }
    if (rb == -1) {
        return -1;
    }
    return crc;
}

/*
  Name  : CRC-8
  Poly  : 0x31    x^8 + x^5 + x^4 + 1
  Init  : 0xFF
  Revert: false
  XorOut: 0x00
  Check : 0xF7 ("123456789")
  MaxLen: 15 bytes(127 bits) - single, double, triple 
    and parity errors detection
*/
unsigned char crc8(unsigned char *pcBlock, unsigned int len, unsigned char init) {
    unsigned char crc = init;
    unsigned int i;

    while (len--)
    {
        crc ^= *pcBlock++;

        for (i = 0; i < 8; i++)
            crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
    }

    return crc;    
}


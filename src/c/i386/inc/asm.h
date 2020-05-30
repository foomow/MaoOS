

#define uint8 unsigned char
#define uint unsigned char
#define byte unsigned char
#define uint16 unsigned short
#define uint32 unsigned int
#define uint64 unsigned long long
#define true 1
#define false 0

void outb(unsigned short port, uint8 val);
uint8 inb(unsigned short port);

void outw(unsigned short port, uint16 val);
uint16 inw(unsigned short port);

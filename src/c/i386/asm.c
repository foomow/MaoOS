
void outb(unsigned short port, uint8 val)
{
	asm volatile ("outb %1, %0" : : "a"(val), "Nd"(port) );
}
uint8 inb(unsigned short port)
{
	uint8 ret;
	asm volatile ("inb %0, %1":"=a"(ret):"Nd"(port) );
	return ret;
}

void outw(unsigned short port, uint16 val)
{
	asm volatile ("outw %1, %0" : : "a"(val), "Nd"(port) );
}
uint16 inw(unsigned short port)
{
	uint16 ret;
	asm volatile ("inw %0, %1":"=a"(ret):"Nd"(port) );
	return ret;
}


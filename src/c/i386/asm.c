
void outb(uint16 port, uint8 val)
{
	asm volatile ("out %1, %0" : : "a"(val), "Nd"(port) );
}
uint8 inb(uint16 port)
{
	uint8 ret;
	asm volatile ("in %0, %1":"=a"(ret):"Nd"(port) );
	return ret;
}

void outw(uint16 port, uint16 val)
{
	asm volatile ("out %1, %0" : : "a"(val), "Nd"(port) );
}

uint16 inw(uint16 port)
{
	uint16 ret;
	asm volatile ("in %0, %1":"=a"(ret):"Nd"(port) );
	return ret;
}

void outd(uint16 port, uint32 val)
{
	asm volatile ("out %1, %0" : : "a"(val), "Nd"(port) );
}

uint32 ind(uint16 port)
{
	uint32 ret;
	asm volatile ("in %0, %1":"=a"(ret):"Nd"(port) );
	return ret;
}


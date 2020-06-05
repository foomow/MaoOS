void get_cmos_date()
{
	outb(0x70,0x0a);
	byte busy=inb(0x71)&0x80;
	while(busy){
		busy=inb(0x71)&0x80;
		}

	outb(0x70,0x00);
	cmos_second=inb(0x71);
	outb(0x70,0x02);
	cmos_minute=inb(0x71);
	outb(0x70,0x04);
	cmos_hour=inb(0x71);
	outb(0x70,0x07);
	cmos_day=inb(0x71);
	outb(0x70,0x08);
	cmos_month=inb(0x71);
	outb(0x70,0x09);
	cmos_year=inb(0x71);	
}

void print_time()
{
	printc('2');
	printc('0');
	print_byte_hex(cmos_year);

	printc('-');
	print_byte_hex(cmos_month);

	printc('-');
	print_byte_hex(cmos_day);
	
	printc(' ');
	print_byte_hex(cmos_hour);
	printc(':');
	print_byte_hex(cmos_minute);
	printc(':');
	print_byte_hex(cmos_second);

}

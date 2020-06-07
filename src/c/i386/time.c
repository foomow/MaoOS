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
uint16 month_days[12]={0,31,59,90,120,151,181,212,243,273,304,334};
uint32 get_tick()//form 2000-01-01 00:00:00
{
	get_cmos_date();
	uint16 leap_year_count=(cmos_year+3)/4;
	if(cmos_year%4==0&&cmos_month>2)leap_year_count++;
	uint16 day_count=cmos_year*365+month_days[cmos_month-1]+cmos_day-1+leap_year_count;
	return day_count*24*60*60+cmos_hour*60*60+cmos_minute*60+cmos_second;
}
void wait_cmos()
{
	uint32 s=get_tick();
}

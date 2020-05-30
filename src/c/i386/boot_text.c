uint16 cur_pos=0;
void cls()
{
	volatile char *video = (volatile char*)0xB8000;
	int i=0;
	int count=25*80;
	while( i< count)
	{       
		*video++ = 0;
		*video++ = 0x0A;
		i++;
	}
	cur_pos=0;
}
void scroll()
{
	volatile char *video = (volatile char*)0xB8F00;

	asm("mov esi,0x480A0");
	asm("mov edi,0x48000");
	asm("mov ecx,0xF00");
	asm("rep movsb");

	for(int i=0;i<80;i++)
	{
		*video++=0;
		*video++=0x0A;
	}
	cur_pos-=80;
}
void prints(const char* str)
{
	volatile char *video;
	while(*str!=0)
	{
		char c=*str++;
		if(c>=0x20&&c<=0x7e)
		{
			video = (char*)(0xB8000+2*cur_pos);
			*video++ = c;
			*video = 0x0F;
			cur_pos++;
		}
		if(c=='\n')
		{
			cur_pos=cur_pos-cur_pos%80+80;
		}
		if(c=='\r')
		{
			cur_pos+=80;
		}
		if(cur_pos>=25*80)scroll();
	}
}
void printc(char c)
{
	volatile char *video = (volatile char*)0xB8000+2*cur_pos;
	*video++ = c;
	*video = 0x0F;
	cur_pos++;
	if(cur_pos>25*80)scroll();
}
void print_cr()
{
	cur_pos=cur_pos-cur_pos%80+80;
	if(cur_pos>=25*80)scroll();
}
void print_u32(uint32 val)
{
	char c[11];
	byte pos=0;
	do{
		char d=val%10;	
		c[pos++]=d+'0';
		val=val/10;
	}while(val>10);
	if(val>0)c[pos++]=val+'0';
	
	while(pos-->0)
	{
		printc(c[pos]);
	}

}
void print_hex_8(uint8 v)
{
	char hex_char[17]="0123456789ABCDEF";

	char idx=(v&0xF0)>>4;
	char c=hex_char[idx];
	printc(c);
	idx=v&0x0F;
	c=hex_char[idx];
	printc(c);
}
void print_hex_16(uint16 v)
{
	print_hex_8((v>>8)&0xFF);
	print_hex_8(v&0xFF);
}
void print_hex_32(uint32 v)
{
	print_hex_8((v>>24)&0xFF);
	print_hex_8((v>>16)&0xFF);
	print_hex_8((v>>8)&0xFF);
	print_hex_8(v&0xFF);
}
void print_hex_64(uint64 v)
{
	print_hex_8((v>>56)&0xFF);
	print_hex_8((v>>48)&0xFF);
	print_hex_8((v>>40)&0xFF);
	print_hex_8((v>>32)&0xFF);
	print_hex_8((v>>24)&0xFF);
	print_hex_8((v>>16)&0xFF);
	print_hex_8((v>>8)&0xFF);
	print_hex_8(v&0xFF);
}




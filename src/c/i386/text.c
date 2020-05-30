

void init_text(){
	for(int i=0;i<256;i++)
		cl_cache[i]=0;
	cur_pos=0;
	cl_pos=0;
}

void enable_cur()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | 14);

	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | 15);
}

void disable_cur()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void update_cur()
{
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8) (cur_pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8) ((cur_pos >> 8) & 0xFF));
}

void cls()
{
	volatile char *video = (volatile char*)0xb8000;
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
	volatile char *video = (volatile char*)0xb8F00;

	asm("mov esi,0xb80A0");
	asm("mov edi,0xb8000");
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
			video = (char*)(0xb8000+2*cur_pos);
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
	volatile char *video = (volatile char*)0xb8000+2*cur_pos;
	*video++ = c;
	*video = 0x0F;
	cur_pos++;
	if(cur_pos>25*80)scroll();
}
void show_cmd()
{
	cur_pos=cur_pos-cur_pos%80;
	volatile char *video = (volatile char*)0xb8000+2*cur_pos;
	*video++ = ':';
	*video++ = 0x0A;
	cur_pos++;
	*video++ = ')';
	*video++ = 0x0A;
	cur_pos++;
	*video++ = ' ';
	*video++ = 0x0A;
	cur_pos++;
	for(int i=0;i<77;i++)
	{
		*video++ = ' ';
		*video++ = 0x0A;
	}
	update_cur();
}
void cmd_input(char c)
{
	cl_cache[cl_pos++]=c;
	printc(c);
	update_cur();
}
void print_cr()
{
	cur_pos=cur_pos-cur_pos%80+80;
	if(cur_pos>=25*80)scroll();
}
void print_byte_hex(char v)
{
	char hex_char[17]="0123456789ABCDEF";

	char idx=(v&0xF0)>>4;
	char c=hex_char[idx];
	printc(c);
	idx=v&0x0F;
	c=hex_char[idx];
	printc(c);
}
void print_word_hex(short v)
{
	print_byte_hex((char)((v&0xff00)>>8));
	print_byte_hex((char)(v&0x00ff));
}

void print_dword_hex(int v)
{
	print_word_hex((short)((v&0xffff0000)>>16));
	print_word_hex((short)(v&0xffff));
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




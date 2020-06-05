
REG get_disk_reg()
{
	REG ret;
	ret.Data=inb(IO_DATA);
	ret.ErrorFeatures=inb(IO_ERR);
	ret.Sector_Count=inb(IO_SEC_C);
	ret.Sector_Number=inb(IO_SEC_N);
	ret.Cylinder_Low=inb(IO_CYL_L);
	ret.Cylinder_High=inb(IO_CYL_H);
	ret.DriveHead=inb(IO_DH);
	ret.StatusCommand=inb(IO_STAT);
	return ret;
}
void show_disk_reg()
{
	REG reg=get_reg();
	char str0[7]="Data: ";
	char str1[11]="     E/F: ";
	char str2[6]="S_C: ";
	char str3[11]="     S_N: ";
	char str4[6]="C_L: ";
	char str5[11]="     C_H: ";
	char str6[6]="D_H: ";
	char str7[11]="     COM: ";

	prints(str0);print_word_hex(reg.Data);
	prints(str1);print_byte_hex(reg.ErrorFeatures);print_cr();
	prints(str2);print_byte_hex(reg.Sector_Count);
	prints(str3);print_byte_hex(reg.Sector_Number);print_cr();
	prints(str4);print_byte_hex(reg.Cylinder_Low);
	prints(str5);print_byte_hex(reg.Cylinder_High);print_cr();
	prints(str6);print_byte_hex(reg.DriveHead);
	prints(str7);print_byte_hex(reg.StatusCommand);print_cr();
}
void show_disk_data(byte drv,uint16 offset)
{
			disable_cur();
			byte buff[512];
			uint16 idx=0;
			uint16 sec_n=offset;
			readsector(buff,drv,sec_n);
			prints("Sector:");
			print_u32(sec_n);
			do{
				for(int i=0;i<256;i++)
				{						
					if(i%16==0)print_cr();
					if(i%8==0)printc(' ');
					print_byte_hex(buff[idx++]);
					printc(' ');
				}
				prints("\npress any key to continue, q to quit.\n");
				char k;
				do{
					k=getkey();							
				}
				while(k==0);
				if(k=='q')break;
				if(idx>=512)
				{
					if(sec_n<0xffff)
					{
						sec_n++;
						idx=0;
						readsector(buff,drv,sec_n);
						print_cr();
						prints("Sector:");
						print_u32(sec_n);
					}
					else
					{
						break;
					}
				}
			}while(1);
			enable_cur();	
}
uint32 get_total_sector(byte drv)
{
	outb(IO_DH,drv);
	outb(IO_SEC_C,0);
	outb(IO_SEC_N,0);
	outb(IO_CYL_L,0);
	outb(IO_CYL_H,0);

	outb(IO_COMM,0xEC);
	byte ret=inb(IO_COMM);
	while((ret&8)==0&&(ret&1)==0&&ret!=0)
	{
		ret=inb(IO_COMM);
	}
	if((ret&1)==0&&ret!=0)
	{
		for(int i=0;i<60;i++)get_reg();
		REG reg=get_reg();
		uint16 low=reg.Data;
		reg=get_reg();
		uint16 high=reg.Data;
		uint32 ret=(high<<16)+low;
		return ret;
	}
	else
	{
		return 0;
	}
}
byte readbyte(byte drv,uint32 sector,uint16 offset)
{
	if(offset>512)
	{
		sector+=offset/0x200;
		offset=offset%0x200;
	}
	byte stat;
	if(drv==DRV_MASTER)
		outb(IO_DH,0xE0);
	else
		outb(IO_DH,0xF0);

	outw(IO_DATA,0);
	outb(IO_SEC_C,1);
	outb(IO_LBA_L,sector&0xFF);
	outb(IO_LBA_M,(sector>>8)&0xFF);
	outb(IO_LBA_H,(sector>>16)&0xFF);
	outb(IO_COMM,0x20);
	stat=inb(IO_COMM);
	while((stat&8)==0&&(stat&1)==0&&stat!=0)
	{
		stat=inb(IO_COMM);
	}
	for(int i=0;i<256;i++)
	{
		uint32 data=inw(IO_DATA);
		if(i==offset/2)
		{
			if(offset%2==0)
				return data&0xFF;
			else
				return data>>8&0xFF;
		}
	}

}
uint16 readint16(byte drv,uint32 sector,uint16 offset)
{
	byte hi=readbyte(drv,sector,offset+1);
	uint16 ret=hi;
	ret=(ret<<8)+readbyte(drv,sector,offset);
	return ret;
}
uint32 readint32(byte drv,uint32 sector,uint16 offset)
{
	byte hi=readbyte(drv,sector,offset+3);	
	uint32 ret=hi;
	ret=(ret<<8)+readbyte(drv,sector,offset+2);
	ret=(ret<<8)+readbyte(drv,sector,offset+1);
	ret=(ret<<8)+readbyte(drv,sector,offset);
	return ret;
}
byte poll_IO()
{
	byte stat;
	stat=inb(IO_STAT);
	while(
		!((stat&0x80)==0&&(stat&0x08)!=0)
		&&!((stat&0x20)!=0||(stat&0x01)!=0)
		)
	{
		stat=inb(IO_STAT);
	}
	return stat;
}
void readsector(byte* buff,byte drv,uint32 sector)
{
	byte stat;
	if(drv==DRV_MASTER)
		outb(IO_DH,0xE0);
	else
		outb(IO_DH,0xF0);

	outw(IO_DATA,0);
	outb(IO_SEC_C,1);
	outb(IO_LBA_L,sector&0xFF);
	outb(IO_LBA_M,(sector>>8)&0xFF);
	outb(IO_LBA_H,(sector>>16)&0xFF);
	outb(IO_COMM,0x20);
	stat=poll_IO();
	for(int i=0;i<256;i++)
	{
		uint32 data=inw(IO_DATA);
		*buff++=data&0xFF;
		*buff++=data>>8&0xFF;
	}
}
void writesector(byte* buff,byte drv,uint32 sector)
{
	byte stat;
	if(drv==DRV_MASTER)
		outb(IO_DH,0xE0);
	else
		outb(IO_DH,0xF0);

	outb(IO_SEC_C,1);
	outb(IO_LBA_L,sector&0xFF);
	outb(IO_LBA_M,(sector>>8)&0xFF);
	outb(IO_LBA_H,(sector>>16)&0xFF);
	outb(IO_COMM,0x30);
	stat=poll_IO();

	for(int i=0;i<256;i++)
	{
		uint16 data=buff[i*2+1];
		data=data<<8;
		data+=buff[i*2];
		outw(IO_DATA,data);
	}
	outb(IO_COMM,0xE7);
}
void writebyte(uint16 offset,byte val,byte drv,uint32 sector)
{
	byte buff[512];
	readsector(buff,drv,sector);
	buff[offset]=val;
	writesector(buff,drv,sector);
}
void write16(uint16 offset,uint16 val,byte drv,uint32 sector)
{
	byte buff[512];
	readsector(buff,drv,sector);
	buff[offset++]=val&0xff;
	buff[offset++]=(val>>8)&0xff;
	writesector(buff,drv,sector);
}
void write32(uint16 offset,uint32 val,byte drv,uint32 sector)
{
	byte buff[512];
	readsector(buff,drv,sector);
	buff[offset++]=val&0xff;
	buff[offset++]=(val>>8)&0xff;
	buff[offset++]=(val>>16)&0xff;
	buff[offset]=(val>>24)&0xff;
	writesector(buff,drv,sector);
}

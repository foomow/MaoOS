#define IO_BASE			 0x1f0
#define IO_DATA			 0x1f0
#define IO_ERR			 0x1f1
#define IO_FEA			 0x1f1
#define IO_SEC_C		 0x1f2
#define IO_SEC_N		 0x1f3
#define IO_LBA_L		 0x1f3
#define IO_CYL_L		 0x1f4
#define IO_LBA_M		 0x1f4
#define IO_CYL_H		 0x1f5
#define IO_LBA_H		 0x1f5
#define IO_DH			 0x1f6
#define IO_STAT			 0x1f7
#define IO_COMM			 0x1f7

#define DRV_MASTER 0xA0
#define DRV_SLAVE 0xB0

typedef struct io_register{
 uint16 Data;
 uint8 ErrorFeatures;
 uint8 Sector_Count;
 uint8 Sector_Number;
 uint8 Cylinder_Low;
 uint8 Cylinder_High;
 uint8 DriveHead;
 uint8 StatusCommand;
} REG;

REG get_reg()
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
	stat=inb(IO_COMM);
	while((stat&8)==0&&(stat&1)==0&&stat!=0)
	{
		stat=inb(IO_COMM);
	}
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
	stat=inb(IO_COMM);
	while((stat&8)==0&&(stat&1)==0&&stat!=0)
	{
		stat=inb(IO_COMM);
	}

	for(int i=0;i<256;i++)
	{

		uint16 data=buff[i*2+1];
		data=data<<8;
		data+=buff[i*2];
		outw(IO_DATA,data);
	}
	outb(IO_COMM,0xE7);
}




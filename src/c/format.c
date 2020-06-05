#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include <string.h>
#include <time.h>

#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fs.h>
#include <unistd.h>

#include "maofs.c"


void main()
{
	struct tm *now;
	time_t t=time(NULL);
	now=localtime(&t);

	now->tm_year+=1900;
	now->tm_mon+=1;

	printf("%d-%02d-%02d %02d:%02d:%02d\n",
			now->tm_year,
			now->tm_mon,
			now->tm_mday,
			now->tm_hour,
			now->tm_min,
			now->tm_sec
	      );

	unsigned long total_sector=0;
	int fd=open("/dev/sdb",O_RDONLY);
	ioctl(fd, BLKGETSIZE, &total_sector);
	close(fd);

	printf("total sector:%d\n",total_sector);

	printf("sys sector:20\n");
	printf("avaliable sector:%d\n",total_sector-20);

	unsigned long ft_sector=(total_sector-20)/1025;
	unsigned long c_sector=total_sector-20-ft_sector;
	if(ft_sector*1024<c_sector)
	{
		ft_sector++;
		c_sector--;
	}

	printf("file table:%d\n",ft_sector);
	printf("content sector:%d\n",c_sector);

	FILE *fp=fopen("/dev/sdb","wb");
	//pass boot segment
	fseek(fp,512*19,SEEK_SET);
	char buff[512];
	memset(buff,0,sizeof(buff));
	unsigned short idx=0;
	//format signiture,use this to detect wheather this disk is formated.
	buff[idx++]=20;
	buff[idx++]=11;
	buff[idx++]=11;
	buff[idx++]=28;

	//format time
	buff[idx++]=now->tm_year/100;
	buff[idx++]=now->tm_year%100;
	buff[idx++]=now->tm_mon;
	buff[idx++]=now->tm_mday;
	buff[idx++]=now->tm_hour;
	buff[idx++]=now->tm_min;
	buff[idx++]=now->tm_sec;

	//disk label
	buff[idx++]='M';
	buff[idx++]='a';
	buff[idx++]='o';
	buff[idx++]='O';
	buff[idx++]='S';
	buff[idx++]=0;

	idx=32;
	//total sector count
	buff[idx++]=total_sector&0xFF;
	buff[idx++]=(total_sector>>8)&0xFF;
	buff[idx++]=(total_sector>>16)&0xFF;
	buff[idx++]=(total_sector>>24)&0xFF;
	//file table sector count
	buff[idx++]=ft_sector&0xFF;
	buff[idx++]=(ft_sector>>8)&0xFF;
	buff[idx++]=(ft_sector>>16)&0xFF;
	buff[idx++]=(ft_sector>>24)&0xFF;
	//content sector count
	buff[idx++]=c_sector&0xFF;
	buff[idx++]=(c_sector>>8)&0xFF;
	buff[idx++]=(c_sector>>16)&0xFF;
	buff[idx++]=(c_sector>>24)&0xFF;

	fwrite(buff,1,sizeof(buff),fp);

	//reset file table
	memset(buff,0,sizeof(buff));
	fseek(fp,512*20,SEEK_SET);
	for(int i=0;i<ft_sector*2;i++)
		fwrite(buff,1,sizeof(buff),fp);

	//setup root dir
	//1 - set file table tag
	fseek(fp,512*20,SEEK_SET);
	buff[0]=0x40;
	fwrite(buff,1,1,fp);
	//2- - setup content
	unsigned int c_sector_idx=20+ft_sector;
	fseek(fp,c_sector_idx*512,SEEK_SET);
	struct file_hd header;
	

	header.c_centry=now->tm_year/100;
	header.c_year=now->tm_year%100;
	header.c_month=now->tm_mon;
	header.c_day=now->tm_mday;
	header.c_hour=now->tm_hour;
	header.c_minute=now->tm_min;
	header.c_second=now->tm_sec;

	header.u_centry=now->tm_year/100;
	header.u_year=now->tm_year%100;
	header.u_month=now->tm_mon;
	header.u_day=now->tm_mday;
	header.u_hour=now->tm_hour;
	header.u_minute=now->tm_min;
	header.u_second=now->tm_sec;
	header.size=0;
	header.attr=0;
	header.reserved=0;

	write_header(fp,&header);	
	//.. and . dir
	char dirinfo[4];
	dirinfo[0]=0;
	dirinfo[1]=0;
	dirinfo[2]=0;
	dirinfo[3]=0;

	fwrite(&dirinfo,1,4,fp);
	
	dirinfo[0]=c_sector_idx&0xFF;
	dirinfo[1]=(c_sector_idx>>8)&0xFF;
	dirinfo[2]=(c_sector_idx>>16)&0xFF;
	dirinfo[3]=(c_sector_idx>>24)&0xFF;
	fwrite(&dirinfo,1,4,fp);

	dirinfo[0]=0xFF;
	dirinfo[1]=0xFF;
	dirinfo[2]=0xFF;
	dirinfo[3]=0xFF;
	fwrite(&dirinfo,1,4,fp);

	fclose(fp);
}


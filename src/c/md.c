#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include "maofs.c"



unsigned int mk_dir(unsigned int parent_sector,char* dir_name,char *des_file_name)
{
	unsigned int des_sector=next_free_sector();
    //change fat for new dir
	set_fat(des_sector,4);

	//update root directory content for new dir here
	FILE *fp_d=fopen(des_file_name,"rb");
	fseek(fp_d,512*parent_sector+44,SEEK_SET);
	unsigned int new_file_sector=0;
	unsigned int new_file_offset=40;
	do{
		if(new_file_offset>=508)
		{
			if(get_fat(parent_sector)==1)set_fat(parent_sector,2);
			parent_sector=next_free_sector();			
			set_fat(parent_sector,1);
			new_file_offset=0;
		}
		else
		{
			new_file_offset+=4;
		}		
		
		fread(&new_file_sector,1,sizeof(new_file_sector),fp_d);
	}
	while(new_file_sector!=0&&new_file_sector!=0xffffffff);
	fclose(fp_d);

	fp_d=fopen(des_file_name,"wb");	
    //move sectors list end tag
	if(new_file_sector==0xffffffff){
		unsigned int end_tag_sector=parent_sector;
		unsigned int end_tag_offset=new_file_offset+4;
		if(end_tag_offset>=508)
		{
			if(get_fat(end_tag_sector)==1)set_fat(end_tag_sector,2);
			end_tag_sector=next_free_sector();
			set_fat(end_tag_sector,1);
			end_tag_offset=0;
		}
		fseek(fp_d,end_tag_sector*512+end_tag_offset,SEEK_SET);
		fwrite(&new_file_sector,1,sizeof(new_file_sector),fp_d);
	}
	printf("parent dir start from sector:%X\n",parent_sector);
	printf("file should start from sector:%X\n",des_sector);
	fseek(fp_d,parent_sector*512+new_file_offset,SEEK_SET);
	fwrite(&des_sector,1,sizeof(des_sector),fp_d);
	
	
	fseek(fp_d,des_sector*512,SEEK_SET);
	struct file_hd header;
	memset(header.name,0,16);
	strcpy(header.name,dir_name);

	struct tm *now;
	time_t t=time(NULL);
	now=localtime(&t);

	now->tm_year+=1900;
	now->tm_mon+=1;

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
	header.attr=0x00;//dir+normal
	header.reserved=0;
	write_header(fp_d,&header);

    //.. and . dir
	char dirinfo[4];
	dirinfo[0]=parent_sector&0xFF;
	dirinfo[1]=(parent_sector>>8)&0xFF;
	dirinfo[2]=(parent_sector>>16)&0xFF;
	dirinfo[3]=(parent_sector>>24)&0xFF;

	fwrite(&dirinfo,1,4,fp_d);
	
	dirinfo[0]=des_sector&0xFF;
	dirinfo[1]=(des_sector>>8)&0xFF;
	dirinfo[2]=(des_sector>>16)&0xFF;
	dirinfo[3]=(des_sector>>24)&0xFF;
	fwrite(&dirinfo,1,4,fp_d);

	dirinfo[0]=0xFF;
	dirinfo[1]=0xFF;
	dirinfo[2]=0xFF;
	dirinfo[3]=0xFF;
	fwrite(&dirinfo,1,4,fp_d);

	fclose(fp_d);
	return des_sector;
}


void main(int count,char** argv)
{
	char dir_name[256];
	char des_file_name[100]="/dev/sdb";

	for(int i=0;i<count;i++)
	{
		if(strcmp("-d",argv[i])==0){
			strcpy(des_file_name,argv[i+1]);			
			i++;
		}
		if(strcmp("-n",argv[i])==0){
			strcpy(dir_name,argv[i+1]);			
			i++;
		}
	}
	if(dir_name[0]==0)
	{
		printf("dir name missed,-n <name>!\n");
		return;
	}

	get_dev_info(des_file_name);
	printf("total sector:%d\n",total_sector_count);
	printf("file table sector:%d\n",fat_sector_count);
	printf("content sector:%d\n",content_sector_count);	

	
	unsigned int parent_sec=root_sector;

	char d_name[255];
	char idx=0;
	printf("create dir [%s]\n",dir_name);
	for(int i=0;i<256;i++)
	{
		d_name[idx]=dir_name[i];		
		if(dir_name[i]==0)
		{
			if(idx>0)
			{
				unsigned int dir_sec= mk_dir(parent_sec,d_name,des_file_name);
				printf("dir [%s] created at %X\n",d_name,dir_sec);
				break;
			}
		}
		else
		{
			if(dir_name[i]=='/')
			{
				d_name[idx]=0;
				if(idx>0)
				{
					parent_sec=get_child_sector(d_name,parent_sec,0x0);
				}
				if(parent_sec==0)
				{
					printf("dir [%s] not exists\n",d_name);
					return;
				}
				idx=0;			
			}
			else
			{
				idx++;
			}
		}
	}
}

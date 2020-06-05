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

void main(int count,char** argv)
{
	char dir_name[100];
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

	struct tm *now;
	time_t t=time(NULL);
	now=localtime(&t);

	now->tm_year+=1900;
	now->tm_mon+=1;

	// FILE *fp_s=fopen(dir_name,"rb");
	// if(fp_s==NULL)
	// {
	// 	printf("source file open error!\n");
	// 	return;
	// }

	// struct stat FileAttrib;

	// if (stat(dir_name, &FileAttrib) < 0)
	// 	printf("File Error\n");
	// else
	// 	printf( "Permissions: %d\n", FileAttrib.st_mode );

	// char *path = realpath(dir_name, NULL);

	// printf("path:%s\n",path);
	// printf("file:%s\n",basename(dir_name));
	// printf("size:%d\n",FileAttrib.st_size);


	get_dev_info(des_file_name);
	printf("total sector:%d\n",total_sector_count);
	printf("file table sector:%d\n",fat_sector_count);
	printf("content sector:%d\n",content_sector_count);
	
    unsigned int root_sector=20+fat_sector_count;
    unsigned int parent_sector=root_sector;//todo: detect parent dir
	unsigned int des_sector=next_free_sector();
    //change fat for new dir
	set_fat(des_sector,4);

	//update root directory content for new dir here
	FILE *fp_d=fopen(des_file_name,"rb");
	fseek(fp_d,512*root_sector+44,SEEK_SET);
	unsigned int new_file_sector=0;
	unsigned int new_file_offset=40;
	do{
		if(new_file_offset>508)
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
		if(end_tag_offset>508)
		{
			if(get_fat(end_tag_sector)==1)set_fat(end_tag_sector,2);
			end_tag_sector=next_free_sector();
			set_fat(end_tag_sector,1);
			end_tag_offset=0;
		}
		fseek(fp_d,end_tag_sector*512+end_tag_offset,SEEK_SET);
		fwrite(&new_file_sector,1,sizeof(new_file_sector),fp_d);
	}
	printf("root dir start from sector:%d\n",root_sector);
	printf("file should start from sector:%d\n",des_sector);
	fseek(fp_d,root_sector*512+new_file_offset,SEEK_SET);
	fwrite(&des_sector,1,sizeof(des_sector),fp_d);
	
	
	fseek(fp_d,des_sector*512,SEEK_SET);
	struct file_hd header;
	memset(header.name,0,16);
	strcpy(header.name,dir_name);
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
	header.size=(unsigned int)FileAttrib.st_size;
	header.attr=0x00;//dir+normal

	write_header(fp_d,&header);

	fseek(fp_s,0,SEEK_SET);
	//write first sector
	char c_buff[512];
	int bcount=fread(c_buff,1,473,fp_s);
	printf("%d bytes read.\n",bcount);
	fwrite(c_buff,1,bcount,fp_d);
	
	while(!feof(fp_s))
	{
		des_sector=next_free_sector();
		set_fat(des_sector,2);
		fwrite(&des_sector,1,sizeof(des_sector),fp_d);
		fseek(fp_d,des_sector*512,SEEK_SET);
		bcount=fread(c_buff,1,508,fp_s);
		//printf("%d bytes read.\n",bcount);
		fwrite(c_buff,1,bcount,fp_d);	
	}
	if(bcount<508)
	{
		set_fat(des_sector,1);
	}


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
	fwrite(&dirinfo,1,4,fp);

	dirinfo[0]=0xFF;
	dirinfo[1]=0xFF;
	dirinfo[2]=0xFF;
	dirinfo[3]=0xFF;
	fwrite(&dirinfo,1,4,fp_d);

	fclose(fp_d);
	fclose(fp_s);
}

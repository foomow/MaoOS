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
	char src_file_name[100];
	char des_file_name[100]="/dev/sdb";
	

	for(int i=0;i<count;i++)
	{
		if(strcmp("-d",argv[i])==0){
			strcpy(des_file_name,argv[i+1]);			
			i++;
		}
		if(strcmp("-s",argv[i])==0){
			strcpy(src_file_name,argv[i+1]);			
			i++;
		}
	}

	struct tm *now;
	time_t t=time(NULL);
	now=localtime(&t);

	now->tm_year+=1900;
	now->tm_mon+=1;

	FILE *fp_s=fopen(src_file_name,"rb");
	if(fp_s==NULL)
	{
		printf("source file open error!\n");
		return;
	}

	struct stat FileAttrib;

	if (stat(src_file_name, &FileAttrib) < 0)
		printf("File Error\n");
	else
		printf( "Permissions: %d\n", FileAttrib.st_mode );

	char *path = realpath(src_file_name, NULL);

	printf("path:%s\n",path);
	printf("file:%s\n",basename(src_file_name));
	printf("size:%d\n",FileAttrib.st_size);


	get_dev_info(des_file_name);
	printf("total sector:%d\n",total_sector_count);
	printf("file table sector:%d\n",fat_sector_count);
	printf("content sector:%d\n",content_sector_count);
	
	unsigned int parent_sector=root_sector;//todo: detect parent dir,parent dir of a file is its dir.
	unsigned int des_sector=next_free_sector();
	set_fat(des_sector,3);

	//update parent directory content for new file here
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

	printf("parent dir start from sector:%d\n",parent_sector);
	printf("file should start from sector:%d\n",des_sector);
	fseek(fp_d,parent_sector*512+new_file_offset,SEEK_SET);
	fwrite(&des_sector,1,sizeof(des_sector),fp_d);
	
	
	fseek(fp_d,des_sector*512,SEEK_SET);
	struct file_hd header;
	memset(header.name,0,16);
	strcpy(header.name,basename(src_file_name));
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
	header.attr=0x0E;//sys+exe
	header.reserved=0;

	write_header(fp_d,&header);

	fseek(fp_s,0,SEEK_SET);
	//write first sector
	char c_buff[512];
	int bcount=fread(c_buff,1,472,fp_s);
	printf("%d bytes read.\n",bcount);
	fwrite(c_buff,1,bcount,fp_d);
	
	while(!feof(fp_s))
	{
		des_sector=next_free_sector();
		set_fat(des_sector,2);
		fwrite(&des_sector,1,sizeof(des_sector),fp_d);
		fseek(fp_d,des_sector*512,SEEK_SET);
		bcount=fread(c_buff,1,508,fp_s);
		fwrite(c_buff,1,bcount,fp_d);	
	}
	if(bcount<508)
	{
		set_fat(des_sector,1);
	}

	//write file end info
	fseek(fp_d,des_sector*512+508,SEEK_SET);
	des_sector=0;
	fwrite(&des_sector,1,sizeof(des_sector),fp_d);


	fclose(fp_d);
	fclose(fp_s);
}

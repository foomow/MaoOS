#include <stdio.h>
#include <string.h>
char dev_name[16];
unsigned int fat_sector_count=0;
unsigned int total_sector_count=0;
unsigned int content_sector_count=0;
unsigned int root_sector=0;

struct file_hd{
	char name[16];
	char c_centry;
	char c_year;
	char c_month;
	char c_day;
	char c_hour;
	char c_minute;
	char c_second;
	char u_centry;
	char u_year;
	char u_month;
	char u_day;
	char u_hour;
	char u_minute;
	char u_second;
	unsigned int size;
	char attr;
	char reserved;
};
void write_header(FILE *fp,struct file_hd* header)
{
fwrite(&header->name,1,sizeof(header->name),fp);
fwrite(&header->c_centry,1,sizeof(header->c_centry),fp);
fwrite(&header->c_year,1,sizeof(header->c_year),fp);
fwrite(&header->c_month,1,sizeof(header->c_month),fp);
fwrite(&header->c_day,1,sizeof(header->c_day),fp);
fwrite(&header->c_hour,1,sizeof(header->c_hour),fp);
fwrite(&header->c_minute,1,sizeof(header->c_minute),fp);
fwrite(&header->c_second,1,sizeof(header->c_second),fp);

fwrite(&header->u_centry,1,sizeof(header->u_centry),fp);
fwrite(&header->u_year,1,sizeof(header->u_year),fp);
fwrite(&header->u_month,1,sizeof(header->u_month),fp);
fwrite(&header->u_day,1,sizeof(header->u_day),fp);
fwrite(&header->u_hour,1,sizeof(header->u_hour),fp);
fwrite(&header->u_minute,1,sizeof(header->u_minute),fp);
fwrite(&header->u_second,1,sizeof(header->u_second),fp);

fwrite(&header->size,1,sizeof(header->size),fp);
fwrite(&header->attr,1,sizeof(header->attr),fp);
fwrite(&header->reserved,1,sizeof(header->reserved),fp);
}
void get_dev_info(char* dev)
{
	int idx=0;
	do{
		dev_name[idx]=*(dev+idx);
		idx++;
		
	}
	while(*(dev+idx)!=0);
	dev_name[idx]=0;
	FILE *fp=fopen(dev_name,"rb");
        fseek(fp,19*512+32,SEEK_SET);
	fread(&total_sector_count,1,sizeof(total_sector_count),fp);
	fread(&fat_sector_count,1,sizeof(fat_sector_count),fp);
	fread(&content_sector_count,1,sizeof(content_sector_count),fp);
	root_sector=20+fat_sector_count;
	fclose(fp);
}
unsigned int next_free_sector()
{
	FILE *fp=fopen(dev_name,"rb");
	fseek(fp,20*512,SEEK_SET);
	unsigned int d_sector_offset=0;
	char ft_slot=0;
	for(int i=0;i<fat_sector_count;i++)
	{
		fread(&ft_slot,1,sizeof(ft_slot),fp);
		if(((ft_slot&0xF0)>>4)==0)
		{
			d_sector_offset=i*2;
			break;
		}else if((ft_slot&0x0F)==0)
		{
			d_sector_offset=i*2+1;
			break;
		}
	}
	fclose(fp);
	return 20+fat_sector_count+d_sector_offset;
}
char set_fat(unsigned int sector_idx,char type)
{
	unsigned int sector_offset=sector_idx-20-fat_sector_count;
	unsigned int offset=sector_offset/2;
	char ft_slot=0;
	FILE *fp=fopen(dev_name,"rb");
	fseek(fp,20*512+offset,SEEK_SET);
	fread(&ft_slot,1,sizeof(ft_slot),fp);
	fclose(fp);
	if(offset*2==sector_offset)
	{
		ft_slot=(type<<4)+(ft_slot&0x0F);
	}else 
	{
		ft_slot=(type&0x0F)+(ft_slot&0xF0);
	}
	fp=fopen(dev_name,"wb");
	fseek(fp,20*512+offset,SEEK_SET);
	fwrite(&ft_slot,1,sizeof(ft_slot),fp);
	fclose(fp);

	return ft_slot;

}
char get_fat(unsigned int sector_idx)
{
	unsigned int sector_offset=sector_idx-20-fat_sector_count;
	unsigned int offset=sector_offset/2;
	char ft_slot=0;
	FILE *fp=fopen(dev_name,"rb");
	fseek(fp,20*512+offset,SEEK_SET);
	fread(&ft_slot,1,sizeof(ft_slot),fp);
	fclose(fp);
	if(offset*2==sector_offset)
	{
		ft_slot=ft_slot&0x0F;
	}else 
	{
		ft_slot=ft_slot&0xF0;
	}
	return ft_slot;
}



unsigned int get_child_sector(char *child_name,unsigned int parent_sec,char mask_attr)
{
	unsigned int ret=0;
	if(parent_sec==0)parent_sec=root_sector;
	FILE *fp=fopen(dev_name,"rb");
	fseek(fp,512*parent_sec+34,SEEK_SET);
	char attr;
	fread(&attr,1,sizeof(attr),fp);
	if((attr&0x03)==0)
	{
		unsigned int child_sec_offset=44;		
		unsigned int child_sec;
		fseek(fp,512*parent_sec+child_sec_offset,SEEK_SET);
		fread(&child_sec,1,sizeof(child_sec),fp);
		while(child_sec!=0xffffffff&&ret==0)
		{
			if(child_sec!=0)
			{
				fseek(fp,512*child_sec+34,SEEK_SET);
				fread(&attr,1,sizeof(attr),fp);
				if((attr&mask_attr)==(attr&03))
				{
					char name[16];
					fseek(fp,512*child_sec,SEEK_SET);
					fread(&name,1,sizeof(name),fp);
					if(strcmp(child_name,name)==0){
						ret= child_sec;
						break;
					}					
				}			
			}
			if((child_sec_offset+4)>508)
			{				
				fseek(fp,512*parent_sec+508,SEEK_SET);
				fread(&parent_sec,1,sizeof(parent_sec),fp);
				if(parent_sec==0)return ret;
				child_sec_offset=0;
			}
			else
			{
				child_sec_offset+=4;
			}
			fseek(fp,512*parent_sec+child_sec_offset,SEEK_SET);
			fread(&child_sec,1,sizeof(child_sec),fp);
		}
	}
	fclose(fp);
	return ret;
}


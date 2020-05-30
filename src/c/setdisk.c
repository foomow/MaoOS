#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<elf.h>
void appendfile(FILE* fp_desc,FILE* fp_src)
{
	char buff[1024];
	fseek(fp_src,0,SEEK_SET);
	while(!feof(fp_src))
	{
		size_t bytecount=fread(buff,1,sizeof(buff),fp_src);
		fwrite(buff,bytecount,1,fp_desc);
		//printf("%d bytes written.\n",bytecount);
	}
}
void appenddata(FILE* fp_desc,char* buff,int length)
{
	fwrite(buff,length,1,fp_desc);
}
void writedisk(const char* device,const char* mbrfile,const char* initfile)
{
	FILE *fp;
	fp=fopen(device,"wb");
	FILE *fp_mbr,*fp_init;

	fp_mbr=fopen(mbrfile,"rb");
	appendfile(fp,fp_mbr);
	printf("MBR installed.\n");

	fp_init=fopen(initfile,"rb");
	appendfile(fp,fp_init);
	printf("init file installed\n");

	fclose(fp);
	fclose(fp_mbr);
	fclose(fp_init);
	return;
}
void resetdisk(const char* device)
{
	printf("reseting device boot...\n");
	unsigned long size=1024*10-512;//reset first 19 sector;20th sector is diskinfo
	FILE *fp;	
	char buff[1];
	buff[0]=0;
	fp=fopen(device,"wb");
	for(int i=0;i<size;i++)
		fwrite(buff,1,1,fp);
	fclose(fp);
	printf("device boot reset OK.(0x%X)\n",size);
}
int main(int argcount,char** argv)
{
	char devicename[100];
	char mbrfile[100];
	char initfile[100];
	int cmd=0;
	for(int i=0;i<argcount;i++)
	{
		if(strcmp("-device",argv[i])==0){
			strcpy(devicename,argv[i+1]);			
			i++;
		}
		if(strcmp("-mfile",argv[i])==0){
			strcpy(mbrfile,argv[i+1]);			
			i++;
		}

		if(strcmp("-ifile",argv[i])==0){
			strcpy(initfile,argv[i+1]);			
			i++;
		}

		if(strcmp("-install",argv[i])==0){
			cmd=2;
		}	
		if(strcmp("-reset",argv[i])==0){
			cmd=3;
		}
	}
	switch(cmd)
	{
		case 2:
			writedisk(devicename,mbrfile,initfile);
			break;
		case 3:
			resetdisk(devicename);
			break;
		default:
			printf("-device [device]\n");			
			printf("-mfile [mbr]\n");
			printf("-ifile [init]\n");
			printf("-install\n");
			printf("-reset\n");
			break;
	}
	return 0;
}

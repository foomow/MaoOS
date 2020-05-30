#include<stdio.h>
#include<string.h>
void showbin();
void main(int count,char** params)
{
	printf("Binary Viewer v1.0.23\n");
	char* filename=NULL;
	for(int i=1;i<count;i++)
	{
		if(!strcmp(params[i],""))
		{
			printf("hello");
		}
		filename=params[i];
	}
	if(filename!=NULL)
	{ 
		showbin(filename);		
	}
	else
	{
		printf("Hi~.This is a binary file viewer, give me a filename please.\n");
	}

}
void showbin(char* filename)
{
	printf("** Reading binary data of '%s' **\n",filename);

	FILE* fp=fopen(filename,"rb");
	char buff[256];
	char txt[17];
	int bytelen;
	unsigned long addr=0;

	while(!feof(fp))
	{
		bytelen=fread(buff,1,sizeof(buff),fp);
		if(bytelen<=0)break;

		printf(" OFFSET   00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F     ASCII\n");
		printf("================================================================================\n");
		for(int i=0;i<sizeof(buff);i++)
		{
			if(i%16==0)
			{

				printf("%08X: ",addr);				
			}
			if(i<bytelen)
			{
				printf("%02X ",(unsigned char)buff[i]);
			}
			else
			{
				buff[i]=0;
				printf("-- ");
			}
			if((i+1)%8==0)
			{
				printf(" ");
			}
			if(buff[i]>=0x20&&buff[i]<=0x7e)
			{
				txt[i%16]=buff[i];
			}
			else
			{
				txt[i%16]='.';
			}
			if((i+1)%16==0)
			{
				printf(" [ %s ]",txt);
				addr+=16;
				printf("\n");

			}

		}
		if(!feof(fp))
		{
			printf("To be continue...q to quit.\n");
			char key=(char)getchar();
			if(key=='q')
			{
				break;
			}
			if(key==';')
			{
				fseek(fp,0x1000,SEEK_CUR);
				addr+=0x1000;
			}
			if(key=='l')
			{
				fseek(fp,0x10000,SEEK_CUR);
				addr+=0x10000;
			}

		}
	}
	fclose(fp);
	printf("Finish.\n");
}


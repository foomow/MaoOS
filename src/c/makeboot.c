#include<stdio.h>
#include<stdlib.h>
#include<elf.h>
#include <string.h>

void main(int count,char** args)
{
	char kernelfile[100];
	char outputfile[100];
	if(count!=3)
	{
		printf("input or output file missed.\n");
		return;
	}
	strcpy(kernelfile,args[1]);			
	strcpy(outputfile,args[2]);			


	int main_offset=-1;
	int keypress_offset=-1;
	int timer_offset=-1;
	FILE *fp_b;     
	fp_b=fopen(kernelfile,"rb");

	Elf32_Ehdr elf_header;
	fread(&elf_header,sizeof(elf_header),1,fp_b);
	//printf("section headers table addr:0x%X\n",elf_header.e_shoff);
	//printf("section count:0x%X\n",elf_header.e_shnum);
	//printf("section name string index:0x%X\n",elf_header.e_shstrndx);

	Elf32_Shdr* sections=calloc(elf_header.e_shnum,sizeof(Elf32_Shdr));
	fseek(fp_b,elf_header.e_shoff,SEEK_SET);
	fread(sections,sizeof(Elf32_Shdr),elf_header.e_shnum,fp_b);

	char* section_names=malloc(sections[elf_header.e_shstrndx].sh_size);
	fseek(fp_b,sections[elf_header.e_shstrndx].sh_offset,SEEK_SET);
	fread(section_names,sections[elf_header.e_shstrndx].sh_size,1,fp_b);

	int text_idx=-1,rel_idx=-1,sym_idx=-1,symname_idx=-1,bss_idx=-1,rodata_idx=-1,data_idx=-1;
	for(int i=0;i<elf_header.e_shnum;i++)
	{
		Elf32_Shdr sec=sections[i];
		//	printf("id:0x%X\n\tname:%s\n\ttype:0x%X\n\taddr:0x%X:0x%X\n\tsize:0x%X\n",i,section_names+sec.sh_name,sec.sh_type,sec.sh_addr,sec.sh_offset,sec.sh_size);
		if(strcmp(section_names+sec.sh_name,".text")==0)
		{
			text_idx=i;
//			printf("\t*** code here ***:0x%X\n",sec.sh_offset);
		}
		if(strcmp(section_names+sec.sh_name,".rel.text")==0)
		{
			rel_idx=i;
//			printf("\t*** relocate info here ***:0x%X\n",sec.sh_offset);
		}
		if(strcmp(section_names+sec.sh_name,".symtab")==0)
		{
			sym_idx=i;
//			printf("\t*** symbols here ***:0x%X\n",sec.sh_offset);
		}
		if(strcmp(section_names+sec.sh_name,".strtab")==0)
		{
			symname_idx=i;
//			printf("\t*** symbols name here ***:0x%X\n",sec.sh_offset);
		}
		if(strcmp(section_names+sec.sh_name,".bss")==0)
		{
			bss_idx=i;
//			printf("\t*** bss here ***:0x%X\n",sec.sh_offset);
		}
		if(strcmp(section_names+sec.sh_name,".rodata")==0)
		{
			rodata_idx=i;
//			printf("\t*** rodata here ***:0x%X\n",sec.sh_offset);
		}
		if(strcmp(section_names+sec.sh_name,".data")==0)
		{
			data_idx=i;
//			printf("\t*** data here ***:0x%X\n",sec.sh_offset);
		}

	}

	Elf32_Sym* symtab=malloc(sections[sym_idx].sh_size);
	fseek(fp_b,sections[sym_idx].sh_offset,SEEK_SET);
	fread(symtab,sections[sym_idx].sh_size,1,fp_b);
	//printf("symtab size:%d\n",sections[sym_idx].sh_size);

	char* sym_names=malloc(sections[symname_idx].sh_size);
	fseek(fp_b,sections[symname_idx].sh_offset,SEEK_SET);
	fread(sym_names,sections[symname_idx].sh_size,1,fp_b);

	//printf("sym_names length:%d\n",sections[symname_idx].sh_size);

	int symcount=sections[sym_idx].sh_size/sizeof(Elf32_Sym);
	unsigned long com_size=0;
	//printf("symcount:%d\n",symcount);
	for(int i=0;i<symcount;i++)
	{		
		//	printf("idx:%d name:%s value:%d\n",i,sym_names+symtab[i].st_name,symtab[i].st_value);
		if(strcmp(sym_names+symtab[i].st_name,"main")==0)
		{
			main_offset=symtab[i].st_value;
//			printf("*** main fun detected ***\n");

		}
		if(strcmp(sym_names+symtab[i].st_name,"_KEYPRESS")==0)
		{
			keypress_offset=symtab[i].st_value+0x4005;
//			printf("*** main fun detected ***\n");

		}
		if(strcmp(sym_names+symtab[i].st_name,"_APIC_TIMER")==0)
		{
			timer_offset=symtab[i].st_value+0x4005;
//			printf("*** main fun detected ***\n");
		}
		if(symtab[i].st_shndx==0xfff2)
		{
			symtab[i].st_value=com_size;
			com_size+=symtab[i].st_size;
			
			
		}
	
	}
	unsigned long code_base=0x4000;
	unsigned long codesize=sections[text_idx].sh_size;

	unsigned long ro_base=code_base+codesize+1;
	unsigned long data_base=ro_base+sections[rodata_idx].sh_size;
	unsigned long bss_base=data_base+sections[data_idx].sh_size;
	unsigned long com_base=0;
	if(rodata_idx!=-1)codesize+=sections[rodata_idx].sh_size;
	if(data_idx!=-1)codesize+=sections[data_idx].sh_size;
	if(bss_idx!=-1)codesize+=sections[bss_idx].sh_size;
	if(com_size>0)
	{
		com_base=code_base+codesize;
	}
	char* codebuff=malloc(codesize);
	fseek(fp_b,sections[text_idx].sh_offset,SEEK_SET);
	fread(codebuff,1,sections[text_idx].sh_size,fp_b);
	if(rodata_idx!=-1)
	{
		fseek(fp_b,sections[rodata_idx].sh_offset,SEEK_SET);
		fread(codebuff+sections[text_idx].sh_size,1,sections[rodata_idx].sh_size,fp_b);
	}
	if(data_idx!=-1)
	{
		fseek(fp_b,sections[data_idx].sh_offset,SEEK_SET);
		fread(codebuff+sections[text_idx].sh_size+sections[rodata_idx].sh_size,1,sections[data_idx].sh_size,fp_b);
	}
	// if(bss_idx!=-1)
	// {
	// 	fseek(fp_b,sections[bss_idx].sh_offset,SEEK_SET);
	// 	fread(codebuff+sections[text_idx].sh_size+sections[rodata_idx].sh_size+sections[data_idx].sh_size,1,sections[bss_idx].sh_size,fp_b);
	// }
	//printf("code base_addr:0x%08X size:%d\n",sections[text_idx].sh_offset,sections[text_idx].sh_size);

	if(rel_idx!=-1)
	{
		Elf32_Rel rel;
		int relcount=sections[rel_idx].sh_size/sizeof(rel);
		fseek(fp_b,sections[rel_idx].sh_offset,SEEK_SET);
		for(int i=0;i<relcount;i++)
		{
//			printf("rel[%d]",i);
			fread(&rel,sizeof(rel),1,fp_b);
			int value=symtab[ELF32_R_SYM(rel.r_info)].st_value;
			long symtype=ELF32_ST_TYPE(symtab[ELF32_R_SYM(rel.r_info)].st_info);
			char valuetype=ELF32_R_TYPE(rel.r_info);
//			printf("\tvaluetype:0x%X\n",valuetype);
//			printf("\tsymtype:0x%X\n",symtype);
//			printf("\tcode offset:0x%X\n",rel.r_offset);
//			printf("\treplace to:0x%X\n",value);
			int* reldest=(int*)(codebuff+rel.r_offset); 
//			printf("sectionname:%s\n",section_names+sections[symtab[ELF32_R_SYM(rel.r_info)].st_shndx].sh_name);
			
				switch(valuetype)
				{
					case 1:
						switch(symtype)
						{
							case STT_OBJECT:
								//printf("======:%X\n",symtab[ELF32_R_SYM(rel.r_info)].st_shndx);
								if(symtab[ELF32_R_SYM(rel.r_info)].st_shndx<0xff)
								{
									if(strcmp(section_names+sections[symtab[ELF32_R_SYM(rel.r_info)].st_shndx].sh_name,".data")==0)
									{
										*reldest=data_base+value+4;
									}
									if(strcmp(section_names+sections[symtab[ELF32_R_SYM(rel.r_info)].st_shndx].sh_name,".bss")==0)
									{
										//printf("======:%d\n",*reldest);
										*reldest=bss_base+value+4;
									}
								}
								if(symtab[ELF32_R_SYM(rel.r_info)].st_shndx==0xfff2)
								{
										*reldest=com_base+value+4;						
								}
								//printf("**\n");
								break;
							case STT_SECTION:
								if(strcmp(section_names+sections[symtab[ELF32_R_SYM(rel.r_info)].st_shndx].sh_name,".rodata")==0)
								{
									
									//printf("\treplace to:0x%X\n",rel.r_offset);
									int* reldest=(int*)(codebuff+rel.r_offset);
									*reldest=*reldest+ro_base+4;
								}
								else
									*reldest=*reldest+value+sections[bss_idx].sh_offset;
								//printf("---\n");
								break;
						}
						break;
					case 2:
						*reldest=value-rel.r_offset-4;
						break;
				}
			
		}
	}

//	printf("Writting output file...");
	FILE* fp_o;
	fp_o=fopen(outputfile,"wb");
	char jmp[5]={0xE9,0x00,0x00,0x00,0x00};
	jmp[1]=main_offset&0xFF;
	jmp[2]=main_offset>>8;
	fwrite(jmp,1,sizeof(jmp),fp_o);
	fwrite(codebuff,1,codesize,fp_o);	
	fclose(fp_o);

	if(keypress_offset!=-1&&timer_offset!=-1)
	{
	FILE* fp_dev=fopen("/dev/sdb","wb");
	fseek(fp_dev,19*512+44,SEEK_SET);
	fwrite(&keypress_offset,1,sizeof(keypress_offset),fp_dev);	
	fwrite(&timer_offset,1,sizeof(timer_offset),fp_dev);	
	fclose(fp_dev);
	}
	printf("kernel done\n");	
	fclose(fp_b);
}

void get_elf_header(char *filename)
{
	prints(filename); print_cr();
	Elf32_Ehdr elf_header;
	uint32 readcount = readfile(filename, 0, &elf_header,sizeof(elf_header));
	print_word_hex(readcount);; print_cr();

	//printf("section headers table addr:0x%X\n",elf_header.e_shoff);
	//printf("section count:0x%X\n",elf_header.e_shnum);
	//printf("section name string index:0x%X\n",elf_header.e_shstrndx);
	prints("section headers table addr:");
	print_word_hex(elf_header.e_shoff);; print_cr();
	prints("section count:");
	print_word_hex(elf_header.e_shnum);; print_cr();
	prints("section name string index:");
	print_word_hex(elf_header.e_shstrndx);; print_cr();
	
}
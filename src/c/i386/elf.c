void get_elf_header(char* filename)
{
	prints(filename); print_cr();
	Elf32_Ehdr elf_header;
	uint32 readcount = readfile(filename, 0, (char*)&elf_header, sizeof(elf_header));
	print_word_hex(readcount);; print_cr();

	prints("section headers table addr:");
	print_word_hex(elf_header.e_shoff);; print_cr();
	prints("section count:");
	print_word_hex(elf_header.e_shnum);; print_cr();
	prints("section name string index:");
	print_word_hex(elf_header.e_shstrndx);; print_cr();

	uint16 section_code_idx = 0xffff;
	uint16 section_rel_idx = 0xffff;
	uint16 section_sym_idx = 0xffff;
	uint16 section_symname_idx = 0xffff;
	uint16 section_bss_idx = 0xffff;
	uint16 section_rodata_idx = 0xffff;
	uint16 section_data_idx = 0xffff;

	Elf32_Shdr section;
	uint16 off = elf_header.e_shoff;
	for (int i=0;i< elf_header.e_shnum;i++)
	{
		readcount = readfile(filename, off,(char*)&section, sizeof(Elf32_Shdr));
		prints("section id:");
		print_dword_hex(i);; print_cr();
		prints("section off:");
		print_dword_hex(section.sh_offset);; print_cr();
		prints("section size:");
		print_dword_hex(section.sh_size);; print_cr();
		prints("=================");; print_cr();
		off = off + readcount;
	}

}
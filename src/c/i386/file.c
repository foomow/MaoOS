void format(byte drv)
{
	/*
	byte buff[512];

	buff[0]=20;
	buff[1]=11;
	buff[2]=11;
	buff[3]=28;

	get_cmos_date();
	buff[4]=0x20;
	buff[5]=cmos_year;
	buff[6]=cmos_month;
	buff[7]=cmos_day;
	buff[8]=cmos_hour;
	buff[9]=cmos_minute;
	buff[10]=cmos_second;

	buff[11]='M';
	buff[12]='a';
	buff[13]='o';
	buff[14]='O';
	buff[15]='S';
	buff[16]=0;

	writesector(buff,drv,19);
	*/
}
void list_dir(char *path)
{
	prints("path:");
	prints(path);
	print_cr();
	uint32 content_sec;
	uint32 data_offset = 44;

	uint32 dir_sector = get_dir_sector(path);
	if (dir_sector == 0)
	{
		prints("path does not exist!\n");
		return;
	}
	content_sec = readint32(DRV_MASTER, dir_sector, data_offset);
	char name[16];
	char attr;
	while (content_sec != 0xffffffff)
	{
		if (content_sec != 0)
		{
			attr = readbyte(DRV_MASTER, content_sec, 34);
			for (int i = 0; i < 16; i++)
			{
				name[i] = readbyte(DRV_MASTER, content_sec, i);
			}
			if ((attr & 3) == 0)
				printc('<');
			prints(name);
			if ((attr & 0x3) == 0)
				printc('>');
			uint16 len = get_len(name);
			if ((attr & 0x3) == 0)
				len += 2;
			len = 16 - len;

			while (len > 0)
			{
				printc(' ');
				len--;
			}
			prints("    ");
			print_byte_hex(attr);
			prints("    ");
			print_u32(readbyte(DRV_MASTER, content_sec, 16));
			print_u32(readbyte(DRV_MASTER, content_sec, 17));
			prints("-");
			byte v = readbyte(DRV_MASTER, content_sec, 18);
			if (v < 10)
				printc('0');
			print_u32(v);
			prints("-");
			v = readbyte(DRV_MASTER, content_sec, 19);
			if (v < 10)
				printc('0');
			print_u32(v);
			prints(" ");
			v = readbyte(DRV_MASTER, content_sec, 20);
			if (v < 10)
				printc('0');
			print_u32(v);
			prints(":");
			v = readbyte(DRV_MASTER, content_sec, 21);
			if (v < 10)
				printc('0');
			print_u32(v);
			prints(":");
			v = readbyte(DRV_MASTER, content_sec, 22);
			if (v < 10)
				printc('0');
			print_u32(v);
			prints("    ");
			if ((attr & 0x3) != 0)
				print_u32(readint16(DRV_MASTER, content_sec, 30));
			print_cr();
		}
		data_offset += 4;
		if (data_offset >= 508)
		{
			uint32 next_sec = readint32(DRV_MASTER, dir_sector, 508);
			if (next_sec == 0)
				return;
			dir_sector = next_sec;
			data_offset = 0;
		}
		content_sec = readint32(DRV_MASTER, dir_sector, data_offset);
	}
}
uint32 get_dir_sector(char *dir_name)
{
	uint32 dir_sector = 20 + readint32(DRV_MASTER, 19, 36);
	char d_name[255];
	char idx = 0;
	for (int i = 0; i < 256; i++)
	{
		d_name[idx] = dir_name[i];
		if (dir_name[i] == 0)
		{
			if (idx > 0)
			{
				dir_sector = get_file_sector(dir_sector, d_name);
			}
			return dir_sector;
		}
		else
		{
			if (dir_name[i] == '/')
			{
				d_name[idx] = 0;
				if (idx > 0)
				{
					dir_sector = get_file_sector(dir_sector, d_name);
				}
				if (dir_sector == 0)
				{
					prints("dir not exists\n");
					prints(d_name);
					print_cr();
					return 0;
				}
				idx = 0;
			}
			else
			{
				idx++;
			}
		}
	}
	return dir_sector;
}
uint32 get_file_sector(uint32 dir_sector, char *filename)
{
	uint32 content_sec;
	uint16 data_offset = 44;
	content_sec = readint32(DRV_MASTER, dir_sector, data_offset);
	char name[16];
	while (content_sec != 0 && content_sec != 0xffffffff)
	{
		for (int i = 0; i < 16; i++)
		{
			name[i] = readbyte(DRV_MASTER, content_sec, i);
		}
		//prints(filename);printc(':');prints(name);printc(':');print_word_hex(data_offset);print_cr();
		if (cmps(name, filename) == 0)
		{
			return content_sec;
		}
		data_offset += 4;
		if (data_offset >= 508)
		{
			uint32 next_sec = readint32(DRV_MASTER, dir_sector, 508);
			if (next_sec == 0)
				return 0;
			dir_sector = next_sec;
			data_offset = 0;
		}
		content_sec = readint32(DRV_MASTER, dir_sector, data_offset);
	}
	return 0;
}
uint16 readfile(char *filename, uint16 offset, byte *buff, uint16 length)
{
	char name_tmp[256];
	get_filename_dir(filename, name_tmp);
	uint32 dir_sector = get_dir_sector(name_tmp);
	get_filename_file(filename, name_tmp);
	uint32 sec = get_file_sector(dir_sector, name_tmp);
	uint16 filesize = readint16(DRV_MASTER, sec, 30);
	if ((length + offset) > filesize)
	{
		if (offset > filesize)
			return 0;
		length = filesize - offset;
	}

	int i = 0;
	if (sec != 0)
	{
		offset += 36;
		for (i = 0; i < length; i++)
		{
			while (offset >= 508)
			{
				uint32 next_sec = readint32(DRV_MASTER, sec, 508);
				if (next_sec == 0)
					return i;
				sec = next_sec;
				offset -= 508;
			}
			buff[i] = readbyte(DRV_MASTER, sec, offset++);
		}
	}
	return i;
}
void movefile(char *src, char *dest)
{
	char str[256];
	get_filename_dir(src, str);
	uint32 src_dir_sector = get_dir_sector(str);
	if (src_dir_sector == 0)
	{
		prints("can't find source dir.\n");
		return;
	}
	get_filename_file(src, str);
	uint32 src_file_sector = get_file_sector(src_dir_sector, str);
	if (src_file_sector == 0)
	{
		prints("can't find source file.\n");
		return;
	}

	get_filename_dir(dest, str);
	uint32 dest_dir_sector = get_dir_sector(str);
	if (dest_dir_sector == 0)
	{
		prints("can't find dest dir.\n");
		return;
	}
	get_filename_file(dest, str);
	if (str[0] != 0)
	{
		uint32 dest_file_sector = get_file_sector(dest_dir_sector, str);
		if (dest_file_sector != 0)
		{
			char attr = get_attr(dest_file_sector);
			if ((attr & 0x03) == 0)
			{
				dest_dir_sector = dest_file_sector;
			}
			else
			{
				prints("dest file exists.\n");
				return;
			}
		}
		else
		{
			print_dword_hex(src_file_sector);
			//rename
			for (int i = 0; i < 16; i++)
			{
				writebyte(i, str[i], DRV_MASTER, src_file_sector);
				if (str[i] == 0)
					break;
			}
		}
	}

	//write
	uint16 data_offset = 44;
	uint32 content_sec = readint32(DRV_MASTER, dest_dir_sector, data_offset);

	while (content_sec != 0xffffffff)
	{
		if (content_sec == 0)
		{
			//insert here
			write32(data_offset, src_file_sector, DRV_MASTER, dest_dir_sector);
			break;
		}
		data_offset += 4;
		if (data_offset >= 508)
		{
			uint32 next_sec = readint32(DRV_MASTER, dest_dir_sector, 508);
			if (next_sec == 0)
				return;
			dest_dir_sector = next_sec;
			data_offset = 0;
		}
		content_sec = readint32(DRV_MASTER, dest_dir_sector, data_offset);
	}

	if (content_sec == 0xffffffff)
	{
		write32(data_offset, src_file_sector, DRV_MASTER, dest_dir_sector);
		if (data_offset >= 508)
		{
			uint32 next_sec = readint32(DRV_MASTER, dest_dir_sector, 508);
			if (next_sec == 0)
				return;
			dest_dir_sector = next_sec;
			data_offset = 0;
		}
		else
		{
			data_offset += 4;
		}
		write32(data_offset, 0xffffffff, DRV_MASTER, dest_dir_sector);
	}

	//remove, it should be a individaul function
	data_offset = 44;
	content_sec = readint32(DRV_MASTER, src_dir_sector, data_offset);
	while (content_sec != 0xffffffff)
	{
		if (content_sec == src_file_sector)
		{
			write32(data_offset, 0, DRV_MASTER, src_dir_sector);
			break;
		}
		data_offset += 4;
		if (data_offset >= 508)
		{
			uint32 next_sec = readint32(DRV_MASTER, src_dir_sector, 508);
			if (next_sec == 0)
				return;
			src_dir_sector = next_sec;
			data_offset = 0;
		}
		content_sec = readint32(DRV_MASTER, src_dir_sector, data_offset);
	}
}

byte get_attr(uint32 sector)
{
	return readbyte(DRV_MASTER, sector, 34);
}
byte set_attr(uint32 sector, byte attr)
{
	writebyte(34, attr, DRV_MASTER, sector);
	return 1;
}

uint32 get_free_sector()
{
	uint32 fat_sector_count = readint32(DRV_MASTER, 19, 36);
	char buff[512];
	uint32 sector = 0;
	for (int i = 0; i < fat_sector_count; i++)
	{
		readsector(buff, DRV_MASTER, 20 + i);
		for (int j = 0; j < 512; j++)
		{
			if (((buff[j] & 0xF0) >> 4) == 0)
			{
				sector = i * 1024 + j * 2;
				break;
			}
			else if ((buff[j] & 0x0F) == 0)
			{
				sector = i * 1024 + j * 2 + 1;
				break;
			}
		}
		if (sector != 0)
			break;
	}
	return 20 + fat_sector_count + sector;
}
char get_fat(uint32 sector)
{
	uint32 fat_sector_count = readint32(DRV_MASTER, 19, 36);
	uint32 off = sector - fat_sector_count - 20;
	uint32 fat_sector = 20 + off / 1024;
	uint32 fat_offset = (off % 1024) / 2;
	byte ret = readbyte(DRV_MASTER, fat_sector, fat_offset);
	if (fat_offset % 2 == 0)
	{
		return (ret & 0xf0) >> 4;
	}
	else
	{
		return ret & 0x0f;
	}
}
void set_fat(uint32 sector, char type)
{
	uint32 fat_sector_count = readint32(DRV_MASTER, 19, 36);
	uint32 off = sector - fat_sector_count - 20;
	uint32 fat_sector = 20 + off / 1024;
	uint32 fat_offset = (off % 1024) / 2;
	byte ret = readbyte(DRV_MASTER, fat_sector, fat_offset);
	if (fat_offset % 2 != 0)
	{
		ret = ret & 0x0f;
		ret = ret | ((type & 0x0f) << 4);
	}
	else
	{
		ret = ret & 0xf0;
		ret = ret | (type & 0x0f);
	}
	writebyte(fat_offset, ret, DRV_MASTER, fat_sector);
}

void makedir(char *name)
{
	char str[256];
	get_filename_dir(name, str);
	uint32 dest_dir_sec = get_dir_sector(str);
	if (dest_dir_sec == 0)
	{
		prints("path error!");
		return;
	}
	get_filename_file(name, str);
	uint32 dest_file_sec = get_file_sector(dest_dir_sec, str);
	if (dest_file_sec != 0)
	{
		prints("path exists!");
		return;
	}

	dest_file_sec = get_free_sector();
	set_fat(dest_file_sec, 4);
	get_cmos_date();
	FILE_HEADER header;
	for (int i = 0; i < 16; i++)
	{
		header.name[i] = str[i];
		if (str[i] == 0)
			break;
	}
	header.c_centry = 20;
	header.c_year = cmos_year;
	header.c_month = cmos_month;
	header.c_day = cmos_day;
	header.c_hour = cmos_hour;
	header.c_minute = cmos_minute;
	header.c_second = cmos_second;

	header.u_centry = 20;
	header.u_year = cmos_year;
	header.u_month = cmos_month;
	header.u_day = cmos_day;
	header.u_hour = cmos_hour;
	header.u_minute = cmos_minute;
	header.u_second = cmos_second;
	header.size = 0;
	header.attr = 0;
	header.reserved = 0;
	char *p = (char *)(&header);
	char buff[512];
	readsector(buff, DRV_MASTER, dest_file_sec);
	int i;
	for (i = 0; i <36; i++)
	{
		buff[i] = *(p + i);
	}
	i=36;
	buff[i++] = dest_dir_sec & 0xff;
	buff[i++] = (dest_dir_sec & 0xff00) >> 8;
	buff[i++] = (dest_dir_sec & 0xff0000) >> 16;
	buff[i++] = (dest_dir_sec & 0xff000000) >> 24;

	buff[i++] = dest_file_sec & 0xff;
	buff[i++] = (dest_file_sec & 0xff00) >> 8;
	buff[i++] = (dest_file_sec & 0xff0000) >> 16;
	buff[i++] = (dest_file_sec & 0xff000000) >> 24;

	buff[i++] = 0xff;
	buff[i++] = 0xff;
	buff[i++] = 0xff;
	buff[i++] = 0xff;
	writesector(buff, DRV_MASTER, dest_file_sec);

	uint16 data_offset = 44;
	uint32 content_sec = readint32(DRV_MASTER, dest_dir_sec, data_offset);

	while (content_sec != 0xffffffff)
	{
		if (content_sec == 0)
		{
			//insert here
			write32(data_offset, dest_file_sec, DRV_MASTER, dest_dir_sec);
			break;
		}
		data_offset += 4;
		if (data_offset >= 508)
		{
			uint32 next_sec = readint32(DRV_MASTER, dest_dir_sec, 508);
			if (next_sec == 0)
				return;
			dest_dir_sec = next_sec;
			data_offset = 0;
		}
		content_sec = readint32(DRV_MASTER, dest_dir_sec, data_offset);
	}

	if (content_sec == 0xffffffff)
	{
		write32(data_offset, dest_file_sec, DRV_MASTER, dest_dir_sec);
		if (data_offset >= 508)
		{
			uint32 next_sec = readint32(DRV_MASTER, dest_dir_sec, 508);
			if (next_sec == 0)
				return;
			dest_dir_sec = next_sec;
			data_offset = 0;
		}
		else
		{
			data_offset += 4;
		}
		write32(data_offset, 0xffffffff, DRV_MASTER, dest_dir_sec);
	}
	prints("success.");
}

void makefile(char *name,char *content)
{
	char str[256];
	get_filename_dir(name, str);
	uint32 dest_dir_sec = get_dir_sector(str);
	if (dest_dir_sec == 0)
	{
		prints("path error!");
		return;
	}
	get_filename_file(name, str);
	uint32 dest_file_sec = get_file_sector(dest_dir_sec, str);
	if (dest_file_sec != 0)
	{
		prints("path exists!");
		return;
	}

	dest_file_sec = get_free_sector();
	set_fat(dest_file_sec, 3);
	get_cmos_date();
	FILE_HEADER header;
	for (int i = 0; i < 16; i++)
	{
		header.name[i] = str[i];
		if (str[i] == 0)
			break;
	}
	header.c_centry = 20;
	header.c_year = cmos_year;
	header.c_month = cmos_month;
	header.c_day = cmos_day;
	header.c_hour = cmos_hour;
	header.c_minute = cmos_minute;
	header.c_second = cmos_second;

	header.u_centry = 20;
	header.u_year = cmos_year;
	header.u_month = cmos_month;
	header.u_day = cmos_day;
	header.u_hour = cmos_hour;
	header.u_minute = cmos_minute;
	header.u_second = cmos_second;
	header.size = get_len(content);
	header.attr = 1;
	header.reserved = 0;
	char *p = (char *)(&header);
	char buff[512];
	readsector(buff, DRV_MASTER, dest_file_sec);
	write_head(buff,header);
	int i=36;
	while(*content!=0)
	{
		buff[i++]=*content++;
	}
	writesector(buff, DRV_MASTER, dest_file_sec);

	uint16 data_offset = 44;
	uint32 content_sec = readint32(DRV_MASTER, dest_dir_sec, data_offset);

	while (content_sec != 0xffffffff)
	{
		if (content_sec == 0)
		{
			//insert here
			write32(data_offset, dest_file_sec, DRV_MASTER, dest_dir_sec);
			break;
		}
		data_offset += 4;
		if (data_offset >= 508)
		{
			uint32 next_sec = readint32(DRV_MASTER, dest_dir_sec, 508);
			if (next_sec == 0)
				return;
			dest_dir_sec = next_sec;
			data_offset = 0;
		}
		content_sec = readint32(DRV_MASTER, dest_dir_sec, data_offset);
	}

	if (content_sec == 0xffffffff)
	{
		write32(data_offset, dest_file_sec, DRV_MASTER, dest_dir_sec);
		if (data_offset >= 508)
		{
			uint32 next_sec = readint32(DRV_MASTER, dest_dir_sec, 508);
			if (next_sec == 0)
				return;
			dest_dir_sec = next_sec;
			data_offset = 0;
		}
		else
		{
			data_offset += 4;
		}
		write32(data_offset, 0xffffffff, DRV_MASTER, dest_dir_sec);
	}
	prints("success.");
}

void del(char *name)
{
	char str[256];
	get_filename_dir(name, str);
	uint32 dest_dir_sec = get_dir_sector(str);
	if (dest_dir_sec == 0)
	{
		prints("path error!");
		return;
	}
	get_filename_file(name, str);
	uint32 dest_file_sec = get_file_sector(dest_dir_sec, str);
	if (dest_file_sec == 0)
	{
		prints("file does not exist!");
		return;
	}

	uint16 data_offset = 44;
	uint32 content_sec = readint32(DRV_MASTER, dest_dir_sec, data_offset);

	while (content_sec != 0xffffffff)
	{
		if (content_sec == dest_file_sec)
		{
			write32(data_offset, 0, DRV_MASTER, dest_dir_sec);
			break;
		}
		data_offset += 4;
		if (data_offset >= 508)
		{
			uint32 next_sec = readint32(DRV_MASTER, dest_dir_sec, 508);
			if (next_sec == 0)
				return;
			dest_dir_sec = next_sec;
			data_offset = 0;
		}
		content_sec = readint32(DRV_MASTER, dest_dir_sec, data_offset);
	}

	set_fat(dest_file_sec, 0);
	uint32 next_sec = readint32(DRV_MASTER, dest_file_sec, 508);
	while (next_sec != 0)
	{
		dest_file_sec = next_sec;
		set_fat(dest_file_sec, 0);
		next_sec = readint32(DRV_MASTER, dest_file_sec, 508);
	}
	prints("success.");
}

void write_head(char *buff,FILE_HEADER header)
{
	byte idx=0;
	for(int i=0;i<16;i++)
	buff[idx++]=header.name[i];
	buff[idx++]=header.c_centry;
	buff[idx++]=header.c_year;
	buff[idx++]=header.c_month;
	buff[idx++]=header.c_day;
	buff[idx++]=header.c_hour;
	buff[idx++]=header.c_month;
	buff[idx++]=header.c_day;
	buff[idx++]=header.u_centry;
	buff[idx++]=header.u_year;
	buff[idx++]=header.u_month;
	buff[idx++]=header.u_day;
	buff[idx++]=header.u_hour;
	buff[idx++]=header.u_month;
	buff[idx++]=header.u_day;

	buff[idx++]=header.size&0xff;
	buff[idx++]=(header.size&0xff00)>>8;
	buff[idx++]=(header.size&0xff0000)>>16;
	buff[idx++]=(header.size&0xff000000)>>24;

	buff[idx++]=header.attr;
	buff[idx++]=0;
}
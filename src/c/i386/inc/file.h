void format(byte drv);
void list_dir(char *path);
uint32 get_dir_sector(char* dirname);
uint32 get_file_sector(uint32 dir_sector, char *filename);
uint16 readfile(char* filename,uint16 offset,byte *buff,uint16 length);

byte get_attr(uint32 sector);
byte set_attr(uint32 sector,byte attr);
void movefile(char* src,char* dest);
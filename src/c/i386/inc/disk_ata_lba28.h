#define IO_BASE			 0x1f0
#define IO_DATA			 0x1f0
#define IO_ERR			 0x1f1
#define IO_FEA			 0x1f1
#define IO_SEC_C		 0x1f2
#define IO_SEC_N		 0x1f3
#define IO_LBA_L		 0x1f3
#define IO_CYL_L		 0x1f4
#define IO_LBA_M		 0x1f4
#define IO_CYL_H		 0x1f5
#define IO_LBA_H		 0x1f5
#define IO_DH			 0x1f6
#define IO_STAT			 0x1f7
#define IO_COMM			 0x1f7

#define DRV_MASTER 0xA0
#define DRV_SLAVE 0xB0


typedef struct file_header{
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
} FILE_HEADER;

typedef struct io_register{
 uint16 Data;
 uint8 ErrorFeatures;
 uint8 Sector_Count;
 uint8 Sector_Number;
 uint8 Cylinder_Low;
 uint8 Cylinder_High;
 uint8 DriveHead;
 uint8 StatusCommand;
} REG;

char *current_path;

REG get_reg();
uint32 get_total_sector(byte drv);


byte readbyte(byte drv,uint32 sector,uint16 offset);
uint16 readint16(byte drv,uint32 sector,uint16 offset);
uint32 readint32(byte drv,uint32 sector,uint16 offset);

uint32 get_total_sector(byte drv);
REG get_disk_reg();
void show_disk_reg();
void show_disk_data(byte drv,uint16 offset);
void writesector(byte* buff,byte drv,uint32 sector);
void readsector(byte* buff,byte drv,uint32 sector);
void format(byte drv);
void list_dir(char *path);
uint16 get_dir_sector(char* dirname);
uint16 get_file_sector(char* filename);
uint16 readfile(char* filename,uint16 offset,byte *buff,uint16 length);
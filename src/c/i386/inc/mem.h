#define BASE_MEM_TB 0x100000
#define BASE_MEM_USER 0x200000
typedef enum mem_block_type
{
    MEM_BLOCK_TYPE_FREE,
    MEM_BLOCK_TYPE_CODE,
    MEM_BLOCK_TYPE_DATA,

} MEM_BLOCK_TYPE;
typedef struct mem_block
{
    uint32 id;
    uint32 owner_pid;//0 means kernel
    MEM_BLOCK_TYPE type;
    uint32 address;
    uint32 size;
    uint32 next_block_id;
} MEM_BLOCK;

typedef struct mem_entry
{
    uint64 base;
    uint64 length;   
    uint32 type; 
    uint32 ext;
} MEM_ENTRY;

uint32 USABLE_MEM;

void init_mem();
void detect_mem();
void list_mem();
char* take_mem(uint32 size,uint32 pid,MEM_BLOCK_TYPE type);
char* take_code_mem(uint32 size,uint32 pid);
char* take_data_mem(uint32 size,uint32 pid);
void append_mem_block();
uint32 free_mem(uint32 addr,uint32 pid);


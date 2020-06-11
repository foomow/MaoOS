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
    uint32 owner_id;//0 means kernel
    MEM_BLOCK_TYPE type;
    uint32 address;
    uint32 length;
    uint32 next_block_id;
} MEM_BLOCK;

void init_mem();
uint32 take_mem(uint32 size,uint32 pid);
void free_mem(uint32 addr,uint32 pid);


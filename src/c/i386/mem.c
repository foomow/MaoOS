void init_mem()
{
    MEM_BLOCK *block=(MEM_BLOCK*)BASE_MEM_TB;
    while(block->next_block_id!=0)
    {
        block=(MEM_BLOCK*)(block->next_block_id*sizeof(MEM_BLOCK)+BASE_MEM_TB);
    }
    take_mem(1,0);
}
uint32 take_mem(uint32 size,uint32 pid)
{
    return 0;
}
void free_mem(uint32 addr,uint32 pid)
{
    return;
}
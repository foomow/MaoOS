void init_mem()
{    
    uint32 *membase=(uint32*)0x2910;
    MEM_ENTRY *mem_entry_base=(MEM_ENTRY*)(membase+1);
    USABLE_MEM=0;
    for(uint32 i=0;i<*membase;i++)
    {        
        MEM_ENTRY entry=*(mem_entry_base+i);       
        if(((uint32)entry.base<0xfec00000)&&((uint32)entry.base>=0x100000)&&((uint32)entry.length>0))USABLE_MEM+=(uint32)entry.length;
    }
    prints("USABLE MEMORY:");
    // print_u32(USABLE_MEM);
    // prints(" bytes = ");
    print_u32(USABLE_MEM/1024);
    prints(" k");
    print_cr();

    MEM_BLOCK *block;
    uint32 blockcount=(BASE_MEM_USER-BASE_MEM_TB)/sizeof(MEM_BLOCK);
    // prints("MEM_BLOCK SIZE:");
    // print_u32(sizeof(MEM_BLOCK));
    // print_cr();
    // prints("MEM_BLOCK COUNT:");
    // print_u32(blockcount);
    // print_cr();
    
    for(uint32 i=0;i<blockcount;i++)
    {
        block=(MEM_BLOCK*)(BASE_MEM_TB+i*sizeof(MEM_BLOCK));
        block->id=i;
        block->owner_pid=0;
        block->type=MEM_BLOCK_TYPE_FREE;
        block->address=0;
        block->size=0;
        block->next_block_id=0;
    }
}
void detect_mem()
{
    uint32 *membase=(uint32*)0x2910;
    prints("MEMORY ENTRY COUNT:");
    print_u32(*membase);
    print_cr();

    prints("Base Address         Length               Type\n");
    prints("===========================================================\n");
    MEM_ENTRY *mem_entry_base=(MEM_ENTRY*)(membase+1);
    for(uint32 i=0;i<*membase;i++)
    {        
        MEM_ENTRY entry=*(mem_entry_base+i);        
        print_qword_hex(entry.base);
        prints("     ");
        print_qword_hex(entry.length);
        prints("     ");
        // issue: case count can't more than 3 ???wtf?
        // switch(entry.type)
        // {
        //     case 1:
        //         prints("Usable");
        //         break;
        //     case 2:
        //         prints("Reserved");
        //         break;
        //     case 3:
        //         prints("ACPI reclaimable");
        //         break;  
        //     // case 4:
        //     //     prints("ACPI NVS");
        //     //     break;  
        //     // case 5:
        //     //     prints("Containing bad");
        //     //     break;          
        // }
        if(entry.type==1)prints("Usable");
        if(entry.type==2)prints("Reserved");
        if(entry.type==3)prints("ACPI reclaimable");
        if(entry.type==4)prints("ACPI NVS");
        if(entry.type==5)prints("Containing bad");
        print_cr();
    }
}
void list_mem()
{    
    MEM_BLOCK *block=(MEM_BLOCK*)BASE_MEM_TB;
    prints("ID       ADDR     SIZE     PID     NEXT     TYPE\n");
    prints("================================================\n");
    while(block->type!=MEM_BLOCK_TYPE_FREE)
    {
        print_dword_hex(block->id);
        printc(' ');
        print_dword_hex(block->address);
        printc(' ');
        print_dword_hex(block->size);
        printc(' ');
        print_dword_hex(block->owner_pid);
        printc(' ');
        print_dword_hex(block->next_block_id);
        printc(' ');
        switch(block->type)
        {
            case MEM_BLOCK_TYPE_FREE:
                prints("FREE ");
                break;
            case MEM_BLOCK_TYPE_CODE:
                prints("CODE ");
                break;
            case MEM_BLOCK_TYPE_DATA:
                prints("DATA ");
                break;
        }
        
        // if(block->type==MEM_BLOCK_TYPE_FREE)
        // prints("FREE ");
        // if(block->type==MEM_BLOCK_TYPE_CODE)
        // prints("CODE ");
        // if(block->type==MEM_BLOCK_TYPE_DATA)
        // prints("DATA ");

        print_cr();
        if(block->next_block_id==0)break;
        block=(MEM_BLOCK*)(BASE_MEM_TB+block->next_block_id*sizeof(MEM_BLOCK));
    }
}
char* take_code_mem(uint32 size,uint32 pid)
{
    return take_mem(size,pid,MEM_BLOCK_TYPE_CODE);
}
char* take_data_mem(uint32 size,uint32 pid)
{
    return take_mem(size,pid,MEM_BLOCK_TYPE_DATA);
}
char* take_mem(uint32 size,uint32 pid,MEM_BLOCK_TYPE type)
{
    char* ret=0;
    MEM_BLOCK *block=(MEM_BLOCK*)BASE_MEM_TB;
    if(block->type==MEM_BLOCK_TYPE_FREE)
    {
        block->size=size;
        block->owner_pid=pid;
        block->type=type;
        block->address=BASE_MEM_USER;
        ret= (char*)(block->address);
    }
    else
    {
        MEM_BLOCK new_block;
        new_block.owner_pid=pid;
        new_block.type=type;
        new_block.size=size;
        while(block->next_block_id!=0)
        {
            MEM_BLOCK *pre_block=block;
            block=(MEM_BLOCK*)(BASE_MEM_TB+block->next_block_id*sizeof(MEM_BLOCK));
            if(block->address-(pre_block->address+pre_block->size)>size)//yes, we will insert a new block here
            {                
                new_block.address=pre_block->address+pre_block->size;
                new_block.next_block_id=block->id;
                append_mem_block(new_block,pre_block);
                ret=(char*)(new_block.address);
            }
        }
        if(ret==0)
        {            
            new_block.address=block->address+block->size;
            new_block.next_block_id=0;
            append_mem_block(new_block,block);
            ret=(char*)(new_block.address);
        }
    }
    return ret;
}
void append_mem_block(MEM_BLOCK new_block,MEM_BLOCK *pre_block)
{
    uint32 id=0;
    MEM_BLOCK* block=(MEM_BLOCK*)(BASE_MEM_TB+id*sizeof(MEM_BLOCK));
    while(block->type!=MEM_BLOCK_TYPE_FREE)
    {
        id++;
        if(id>(BASE_MEM_USER-BASE_MEM_TB)/sizeof(MEM_BLOCK))//block count execed.
            return;
        block=(MEM_BLOCK*)(BASE_MEM_TB+id*sizeof(MEM_BLOCK));
    }
    block->owner_pid=new_block.owner_pid;
    block->type=new_block.type;
    block->address=new_block.address;
    block->size=new_block.size;
    pre_block->next_block_id=block->id;
}
uint32 free_mem(uint32 addr,uint32 pid)
{
    MEM_BLOCK* block=(MEM_BLOCK*)(BASE_MEM_TB);
    MEM_BLOCK* pre_block= 0;
    uint32 id=block->id;
    if(block->owner_pid==pid&&block->address==addr)//if its the first block
    {
        if(block->next_block_id==0)
        {
            block->type=MEM_BLOCK_TYPE_FREE;
        }
        else
        {
            MEM_BLOCK* next_block=(MEM_BLOCK*)(BASE_MEM_TB+block->next_block_id*sizeof(MEM_BLOCK));
            block->owner_pid=next_block->owner_pid;
            block->type=next_block->type;
            block->address=next_block->address;
            block->size=next_block->size;
            block->next_block_id=next_block->next_block_id;
            next_block->type=MEM_BLOCK_TYPE_FREE;
        }
    }
    else
    {
        while(block->next_block_id!=0)   
        {
            pre_block=block;
            block=(MEM_BLOCK*)(BASE_MEM_TB+block->next_block_id*sizeof(MEM_BLOCK));
            if(block->owner_pid==pid&&block->address==addr)//hit
            {
                pre_block->next_block_id=block->next_block_id;
                block->type=MEM_BLOCK_TYPE_FREE;
                id=block->id;
                return id;
            }            
        }
    }
    return id;
}
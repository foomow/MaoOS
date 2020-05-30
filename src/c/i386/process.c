void _APIC_TIMER()
{}
void init_process()
{
    for(int i=0;i<MAX_PROCESS;i++)
    {
        process_pool[i].id=0;
    }
}
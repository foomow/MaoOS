#define MAX_PROCESS 0xffff
typedef struct process
{
    unsigned short id;
} Process;

Process process_pool[MAX_PROCESS];
void _APIC_TIMER();
void init_process();


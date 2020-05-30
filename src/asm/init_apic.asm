apic_predefine:
    %define apic  0xFEE00000
    %define IOREGSEL  0xFEC00000
    %define IOWIN  0xFEC00010

    %define APIC_APICID	 0x20
    %define APIC_APICVER	 0x30
    %define APIC_TASKPRIOR	 0x80
    %define APIC_EOI	 0x0B0
    %define APIC_LDR	 0x0D0
    %define APIC_DFR	 0x0E0
    %define APIC_SPURIOUS	 0x0F0
    %define APIC_ESR	 0x280
    %define APIC_ICRL	 0x300
    %define APIC_ICRH	 0x310
    %define APIC_LVT_TMR	 0x320
    %define APIC_LVT_PERF	 0x340
    %define APIC_LVT_LINT0	 0x350
    %define APIC_LVT_LINT1	 0x360
    %define APIC_LVT_ERR	 0x370
    %define APIC_TMRINITCNT	 0x380
    %define APIC_TMRCURRCNT	 0x390
    %define APIC_TMRDIV	 0x3E0
    %define APIC_LAST	 0x38F
    %define APIC_DISABLE	 0x10000
    %define APIC_SW_ENABLE	 0x100
    %define APIC_CPUFOCUS	 0x200
    %define APIC_NMI	 (4<<8)
    %define TMR_PERIODIC	 0x20000
    %define TMR_BASEDIV	 (1<<20)
_apic_map:
		;initialize LAPIC to a well known state
		mov dword [apic+APIC_DFR], 0FFFFFFFFh
		mov eax, dword [apic+APIC_LDR]
		and eax, 00FFFFFFh
		or al, 1
		mov dword [apic+APIC_LDR], eax
		mov dword [apic+APIC_LVT_TMR], APIC_DISABLE
		mov dword [apic+APIC_LVT_PERF], APIC_NMI
		mov dword [apic+APIC_LVT_LINT0], APIC_DISABLE
		mov dword [apic+APIC_LVT_LINT1], APIC_DISABLE
		mov dword [apic+APIC_TASKPRIOR], 0
		mov dword [apic+APIC_SPURIOUS],0x01ff

		call _apic_init_timer
		call _apic_init_keyboard

		;enable APIC
		mov			ecx, 1bh
		rdmsr
		bts			eax, 11
		wrmsr

    ret

_apic_init_keyboard:
		mov dword [IOREGSEL],0x12
		mov dword [IOWIN],0x21
		mov dword [IOREGSEL],0x13
		mov dword [IOWIN],0x00
	ret

_apic_init_timer:
		;map APIC timer to an interrupt, and by that enable it in one-shot mode
		mov			dword [apic+APIC_LVT_TMR], 32
		;set up divide value to 16
		mov			dword [apic+APIC_TMRDIV], 03h

        mov dword [apic+APIC_TMRINITCNT], 0x200000
		;finally re-enable timer in periodic mode
		mov			dword [apic+APIC_LVT_TMR], 32 | TMR_PERIODIC
		mov			dword [apic+APIC_TMRDIV], 03h
	ret
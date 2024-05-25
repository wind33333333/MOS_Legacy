org 0x10000
startloader:
    nop
    nop                 ;loader引导标记
    mov ax,cs
    mov ds,ax
    mov es,ax
    mov sp,0            ;stack up 0x10000

    mov ecx,0x1B
    rdmsr
    test eax,0x100       ;測試bit8 1bsp 0ap
    jz setgdt

	mov	ax,	1301h
	mov	bx,	000fh
	mov	dx,	0100h		;row 2
	mov	cx,	12
	push	ax
	mov	ax,	cs
	mov ds, ax
	mov	es,	ax
	pop	ax
	mov	bp,	StartLoaderMessage
	int	10h

;=======	get memory address size type
	mov	ax,	1301h
	mov	bx,	000Fh
	mov	dx,	0200h		;row 3
	mov	cx,	44
	push	ax
	mov	ax,	cs
	mov ds, ax
	mov	es,	ax
	pop	ax
	mov	bp,	StartGetMemStructMessage
	int	10h

	mov	ebx,0
	mov	ax,	0x00
	mov	es,	ax
	mov ds, ax
	mov	di,	MemoryStructBufferAddr
	mov	dword	[MemStructNumber],	0

Label_Get_Mem_Struct:
	mov	eax,	0x0E820
	mov	ecx,	20
	mov	edx,	0x534D4150
	int	15h
	jc	Label_Get_Mem_Fail
	add	di,	20
	inc	dword	[MemStructNumber]

	cmp	ebx,	0
	jne	Label_Get_Mem_Struct
	jmp	Label_Get_Mem_OK

Label_Get_Mem_Fail:
	mov	dword	[MemStructNumber],	0
	mov	ax,	1301h
	mov	bx,	008Ch
	mov	dx,	0300h		;row 4
	mov	cx,	23
	push	ax
	mov	ax,	cs
	mov ds, ax
	mov	es,	ax
	pop	ax
	mov	bp,	GetMemStructErrMessage
	int	10h
memhlt:
    hlt
    jmp memhlt

Label_Get_Mem_OK:
	mov	ax,	1301h
	mov	bx,	000Fh
	mov	dx,	0300h		;row 4
	mov	cx,	29
	push	ax
	mov	ax,	cs
	mov ds, ax
	mov	es,	ax
	pop	ax
	mov	bp,	GetMemStructOKMessage
	int	10h

;========获取vbe缓冲区地址
	mov ax,0
	mov es,ax
	mov di,VBEStructBufferAddr
	mov ax,4f01h
	mov cx,118h
	int 10h
;=======	set the SVGA mode(VESA VBE)
	mov	ax,	4F02h
	mov	bx,	4180h	;========================mode : 虚拟机0x180 or 物理机0x118
	int 	10h

	cmp	ax,	004Fh
    je setgdt
setvbeerr:
    hlt
    jmp setvbeerr

;=======	init GDT goto protect mode
setgdt:
	cli			;======close interrupt
	lgdt	[GdtPtr32]
	mov	eax,	cr0
	or	eax,	1
	mov	cr0,	eax
	jmp	dword SelectorCode32:GO_TO_TMP_Protect

[bits 32]
GO_TO_TMP_Protect:
    mov	ax,	SelectorData32
    mov	ds,	ax
    mov	es,	ax
    mov	fs,	ax
    mov	ss,	ax
    mov	esp,stackoffset32

;=======	init temporary page table 0x90000   1G大页
	mov	dword	[0x90000],	0x91007
	mov	dword	[0x90800],	0x91007
	mov	dword	[0x91000],	    0x00000083      ;1G
	mov	dword	[0x91000+8],    0x40000083      ;2G
	mov	dword	[0x91000+16],   0x80000083      ;3G
	mov	dword	[0x91000+24],   0xC0000083      ;4G
	mov	dword	[0x90FF8],	    0x90007         ;递归映射 pml4基址0xFFFFFFFFFFFFF000 pdpt基址FFFFFFFFFFE00000

;=======	load GDTR
	lgdt	[GdtPtr64]

;=======	open PAE
	mov	eax,	cr4
    or  eax,    0x50620          ;bit5 PAE, bit9 OSFXSR, bit10 OSXMMEXCPT, bit16 FSGSBASE, bit18 OSXSAVE
	mov	cr4,	eax

;========== 启用avx和avx512指令集
    mov ecx, 0          ; XCR0寄存器
    xor eax, eax
    xor edx, edx
    xgetbv              ; 获取当前的XCR0值
    or  eax, 0xE7       ; 设置位0（XMM）、位1（YMM）、位2（ZMM_Hi256）和位5（ZMM16_31）
    xsetbv              ; 更新XCR0寄存器

;=======	load cr3
	mov	eax,	0x90000
	mov	cr3,	eax

;=======	enable long-mode
	mov	ecx,	0C0000080h		;IA32_EFER
	rdmsr
	or	eax,	101h            ;bit8开启长模式 bit0开启syscall/sysret指令
	wrmsr

;=======	open PE and paging
	mov	eax,	cr0
;	bts	eax,	31            ;bit31 PG位
    or  eax,    0x80000003    ;bit0 PE, bit1 MP, bit31 PG
    btr	eax,	2
	mov	cr0,	eax
	jmp	SelectorCode64:longx64

[bits 64]
longx64:
	mov	ax,	SelectorData64
	mov	ds,	ax
	mov	es,	ax
	mov	fs,	ax
	mov	gs,	ax
	mov	ss,	ax
	mov	rsp,stackoffset64

	mov ecx,0x1B
    rdmsr
    test eax,0x100             ;測試bit8 1bsp 0ap
    jz ap                      ;ap核跳入kernel.bin

    mov     rdi,kerneladdr
    mov     rsi,KernelFileName
    call    read_file
ap:

    mov     rax,kernelstart
    jmp     rax             ;bsp核跳入kernel.bin

    %include    "./bootloader/readdisk.inc"
    %include    "./bootloader/readfile.inc"


;===字符常量
MemStructNumber         equ 0x500                  ;内存结构数量
MemoryStructBufferAddr  equ 0x504
VBEStructBufferAddr     equ 0x600
kerneladdr              equ 0x100000
kernelstart             equ 0x100000+0xffff800000000000
stackoffset32           equ 0x10000
stackoffset64           equ 0x10000+0xffff800000000000                ;临时栈指针



;=================== GDT32==========
align 8
LABEL_GDT32:		dd	0,0
LABEL_DESC_CODE32:	dd	0x0000FFFF,0x00CF9A00
LABEL_DESC_DATA32:	dd	0x0000FFFF,0x00CF9200

GdtLen32	equ	$ - LABEL_GDT32
GdtPtr32	dw	GdtLen32 - 1
	        dd	LABEL_GDT32	;be carefull the address(after use org)!!!!!!

SelectorCode32	equ	LABEL_DESC_CODE32 - LABEL_GDT32
SelectorData32	equ	LABEL_DESC_DATA32 - LABEL_GDT32

;=================== GDT64==========
align 8
LABEL_GDT64:		dq	0x0000000000000000
LABEL_DESC_CODE64:	dq	0x0020980000000000
LABEL_DESC_DATA64:	dq	0x0000920000000000

GdtLen64	equ	$ - LABEL_GDT64
GdtPtr64	dw	GdtLen64 - 1
		    dd	LABEL_GDT64

SelectorCode64	equ	LABEL_DESC_CODE64 - LABEL_GDT64
SelectorData64	equ	LABEL_DESC_DATA64 - LABEL_GDT64

;=======	display messages
StartLoaderMessage:	        db	"Start Loader"
KernelFileName:		        db	"KERNEL  BIN"
StartGetMemStructMessage:	db	"Start Get Memory Struct (address,size,type)."
GetMemStructErrMessage:	    db	"Get Memory Struct ERROR"
GetMemStructOKMessage:	    db	"Get Memory Struct SUCCESSFUL!"


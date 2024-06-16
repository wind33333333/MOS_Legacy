//===============================初始化程序代码=================================================
.section    .init_text, "ax"
.globl _start
_start:
//===============================bsp/ap核
    movl    $0x1B,%ecx           //IA32_APIC_BASE=0x1b 寄存器
    rdmsr
    or      $0xc00,%eax
    wrmsr
    and     $0x100,%eax          //測試bit8 1bsp 0ap
    movabs  %eax,bsp_flags
    jz      kernel_stack_init

//===============================cpu_info.croes_num=cpu数量
    mov        $1,%eax
    cpuid
    shr        $16,%ebx         //右移16位得到cpu数量
    mov        %ebx,%eax
    movabs     %eax,cpu_info

//===============================开辟多核栈空间
    movabs     cpu_info,%eax
    shl        $14,%rax                 //每个核心分配16k栈
    mov        %rax,%rbx
    movabs     kenelstack_top,%rax
    add        %rbx,%rax
    movabs     %rax,kenelstack_top       //内核栈顶

//===============================初始化多核内核栈
kernel_stack_init:
    mov     $0x802,%ecx
    rdmsr                           //获取APIC ID
    shl     $14,%rax                //每个核心分配16k栈
    movabs  $kenelstack_top,%rbx
    mov     (%rbx),%rsp
    sub     %rax,%rsp               //计算每个内核栈 栈顶=VBASESTACK-(APIC ID*16K)

	jmp	    Kernel_init             //进入C语言Kernel_init

//===============================初始化程序数据=================================================
.section    .init_data, "wa"

    .global   IDT_POINTER
    IDT_POINTER:
    IDT_LIMIT:	.word	IDT_END - IDT_Table - 1
    IDT_BASE:	.quad	IDT_Table

    .globl kenelstack_top
    kenelstack_top:   .quad   _end

    .globl bsp_flags
    bsp_flags:       .int   0

    .equ    VBASEADDR,0xffff800000000000      //内核虛擬地址基址


//========================================正式内核代码=================================================

.section    .text, "ax"

//默认中断处理程序
.globl ignore_int
ignore_int:
	cld
	pushq	%rax
	pushq	%rcx
	pushq	%rdx
	pushq	%rbx
	pushq	%rbp
	pushq	%rdi
	pushq	%rsi

	pushq	%r8
	pushq	%r9
	pushq	%r10
	pushq	%r11
	pushq	%r12
	pushq	%r13
	pushq	%r14
	pushq	%r15

	movq	%es,	%rax
	pushq	%rax
	movq	%ds,	%rax
	pushq	%rax

	movq	$0x10,	%rax
	movq	%rax,	%ds
	movq	%rax,	%es

	movabs	$int_msg,%rax			/* leaq get address */
	pushq	%rax
	movq	%rax,	%rdx
	movq	$0x00000000,	%rsi
	movq	$0x00ff0000,	%rdi
	movq	$0,	%rax
	callq	color_printk
	addq	$0x8,	%rsp


	jmp	.

	popq	%rax
	movq	%rax,	%ds
	popq	%rax
	movq	%rax,	%es

	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%r11
	popq	%r10
	popq	%r9
	popq	%r8

	popq	%rsi
	popq	%rdi
	popq	%rbp
	popq	%rbx

//	in       $0x60,%al            //读取键盘状态

    mov     $0,%edx
    mov     $0,%eax
    mov     $0x80B,%ecx
    wrmsr                   //给local apic发送EOI

	popq	%rdx
	popq	%rcx
	popq	%rax
	iretq

int_msg:    .asciz "Unknown interrupt or fault at RIP\n"


//========================================正式内核数据=================================================
.section    .data, "wa"

    .globl IDT_Table1
    IDT_Table1:
    .globl IDT_Table
    IDT_Table:
    	.fill  0x1000-(.-IDT_Table),1,0
    IDT_END:

    .globl __PML4T
    __PML4T:
    	.quad	0
    	.fill	0xFF8-(. -__PML4T),1,0
    	.quad   (__PML4T- VBASEADDR)+0x7

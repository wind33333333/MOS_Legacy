#ifndef __LIB_H__
#define __LIB_H__


#define BOCHS_DG()    __asm__ __volatile__ ("xchg %%bx,%%bx \n\t":: :);

#define PML4_VBASE    0xFFFFFFFFFFFFF000     //pml4虚拟地址基址
#define PDPT_VBASE    0xFFFFFFFFFFE00000     //pdpt虚拟地址基址
#define PD_VBASE      0xFFFFFFFFC0000000     //pd虚拟地址基址
#define PT_VBASE      0xFFFFFF8000000000     //pt虚拟地址基址

extern unsigned int bsp_flags;
extern unsigned int cpu_num;
extern unsigned long kenelstack_top;
extern unsigned long _start_text;




//自旋锁
#define SPIN_LOCK(lock) \
    do {                \
    __asm__ __volatile__ ( \
        "mov $1,%%bl   \n\t" \
        "1:\tmov $0,%%al   \n\t" \
        "lock           \n\t" \
        "cmpxchg %%bl,%0 \n\t" \
        "jnz 1b          \n\t" \
        ::"m"(lock):"%rax","%rbx"); \
    } while(0)

#define NULL 0

#define container_of(ptr, type, member)                            \
({                                            \
    typeof(((type *)0)->member) * p = (ptr);                    \
    (type *)((unsigned long)p - (unsigned long)&(((type *)0)->member));        \
})


#define sti()        __asm__ __volatile__ ("sti	\n\t":::"memory")
#define cli()        __asm__ __volatile__ ("cli	\n\t":::"memory")
#define nop()        __asm__ __volatile__ ("nop	\n\t")
#define io_mfence()    __asm__ __volatile__ ("mfence	\n\t":::"memory")


struct List {
    struct List *prev;
    struct List *next;
};

void list_init(struct List *list) {
    list->prev = list;
    list->next = list;
}

void list_add_to_behind(struct List *entry, struct List *new)    ////add to entry behind
{
    new->next = entry->next;
    new->prev = entry;
    new->next->prev = new;
    entry->next = new;
}

void list_add_to_before(struct List *entry, struct List *new)    ////add to entry behind
{
    new->next = entry;
    entry->prev->next = new;
    new->prev = entry->prev;
    entry->prev = new;
}

void list_del(struct List *entry) {
    entry->next->prev = entry->prev;
    entry->prev->next = entry->next;
}

long list_is_empty(struct List *entry) {
    if (entry == entry->next && entry->prev == entry)
        return 1;
    else
        return 0;
}

struct List *list_prev(struct List *entry) {
    if (entry->prev != NULL)
        return entry->prev;
    else
        return NULL;
}

struct List *list_next(struct List *entry) {
    if (entry->next != NULL)
        return entry->next;
    else
        return NULL;
}

/*
		From => To memory copy Num bytes
*/

void *memcpy(void *From, void *To, long Num) {
    int d0, d1, d2;
    __asm__ __volatile__    (    "cld	\n\t"
                                 "rep	\n\t"
                                 "movsq	\n\t"
                                 "testb	$4,%b4	\n\t"
                                 "je	1f	\n\t"
                                 "movsl	\n\t"
                                 "1:\ttestb	$2,%b4	\n\t"
                                 "je	2f	\n\t"
                                 "movsw	\n\t"
                                 "2:\ttestb	$1,%b4	\n\t"
                                 "je	3f	\n\t"
                                 "movsb	\n\t"
                                 "3:	\n\t"
            :"=&c"(d0), "=&D"(d1), "=&S"(d2)
            :"0"(Num / 8), "q"(Num), "1"(To), "2"(From)
            :"memory"
            );
    return To;
}

/*
		FirstPart = SecondPart		=>	 0
		FirstPart > SecondPart		=>	 1
		FirstPart < SecondPart		=>	-1
*/

int memcmp(void *FirstPart, void *SecondPart, long Count) {
    register int __res;

    __asm__ __volatile__    (    "cld	\n\t"        //clean direct
                                 "repe	\n\t"        //repeat if equal
                                 "cmpsb	\n\t"
                                 "je	1f	\n\t"
                                 "movl	$1,	%%eax	\n\t"
                                 "jl	1f	\n\t"
                                 "negl	%%eax	\n\t"
                                 "1:	\n\t"
            :"=a"(__res)
            :"0"(0), "D"(FirstPart), "S"(SecondPart), "c"(Count)
            :
            );
    return __res;
}

/*
		set memory at Address with C ,number is Count
*/

void *memset(void *Address, unsigned char C, long Count) {
    int d0, d1;
    unsigned long tmp = C * 0x0101010101010101UL;
    __asm__ __volatile__    (    "cld	\n\t"
                                 "rep	\n\t"
                                 "stosq	\n\t"
                                 "testb	$4, %b3	\n\t"
                                 "je	1f	\n\t"
                                 "stosl	\n\t"
                                 "1:\ttestb	$2, %b3	\n\t"
                                 "je	2f\n\t"
                                 "stosw	\n\t"
                                 "2:\ttestb	$1, %b3	\n\t"
                                 "je	3f	\n\t"
                                 "stosb	\n\t"
                                 "3:	\n\t"
            :"=&c"(d0), "=&D"(d1)
            :"a"(tmp), "q"(Count), "0"(Count / 8), "1"(Address)
            :"memory"
            );
    return Address;
}

/*
		string copy
*/

char *strcpy(char *Dest, char *Src) {
    __asm__ __volatile__    (    "cld	\n\t"
                                 "1:	\n\t"
                                 "lodsb	\n\t"
                                 "stosb	\n\t"
                                 "testb	%%al,	%%al	\n\t"
                                 "jne	1b	\n\t"
            :
            :"S"(Src), "D"(Dest)
            :

            );
    return Dest;
}

/*
		string copy number bytes
*/

char *strncpy(char *Dest, char *Src, long Count) {
    __asm__ __volatile__    (    "cld	\n\t"
                                 "1:	\n\t"
                                 "decq	%2	\n\t"
                                 "js	2f	\n\t"
                                 "lodsb	\n\t"
                                 "stosb	\n\t"
                                 "testb	%%al,	%%al	\n\t"
                                 "jne	1b	\n\t"
                                 "rep	\n\t"
                                 "stosb	\n\t"
                                 "2:	\n\t"
            :
            :"S"(Src), "D"(Dest), "c"(Count)
            :
            );
    return Dest;
}

/*
		string cat Dest + Src
*/

char *strcat(char *Dest, char *Src) {
    __asm__ __volatile__    (    "cld	\n\t"
                                 "repne	\n\t"
                                 "scasb	\n\t"
                                 "decq	%1	\n\t"
                                 "1:	\n\t"
                                 "lodsb	\n\t"
                                 "stosb	\n\r"
                                 "testb	%%al,	%%al	\n\t"
                                 "jne	1b	\n\t"
            :
            :"S"(Src), "D"(Dest), "a"(0), "c"(0xffffffff)
            :
            );
    return Dest;
}

/*
		string compare FirstPart and SecondPart
		FirstPart = SecondPart =>  0
		FirstPart > SecondPart =>  1
		FirstPart < SecondPart => -1
*/

int strcmp(char *FirstPart, char *SecondPart) {
    register int __res;
    __asm__ __volatile__    (    "cld	\n\t"
                                 "1:	\n\t"
                                 "lodsb	\n\t"
                                 "scasb	\n\t"
                                 "jne	2f	\n\t"
                                 "testb	%%al,	%%al	\n\t"
                                 "jne	1b	\n\t"
                                 "xorl	%%eax,	%%eax	\n\t"
                                 "jmp	3f	\n\t"
                                 "2:	\n\t"
                                 "movl	$1,	%%eax	\n\t"
                                 "jl	3f	\n\t"
                                 "negl	%%eax	\n\t"
                                 "3:	\n\t"
            :"=a"(__res)
            :"D"(FirstPart), "S"(SecondPart)
            :
            );
    return __res;
}

/*
		string compare FirstPart and SecondPart with Count Bytes
		FirstPart = SecondPart =>  0
		FirstPart > SecondPart =>  1
		FirstPart < SecondPart => -1
*/

int strncmp(char *FirstPart, char *SecondPart, long Count) {
    register int __res;
    __asm__ __volatile__    (    "cld	\n\t"
                                 "1:	\n\t"
                                 "decq	%3	\n\t"
                                 "js	2f	\n\t"
                                 "lodsb	\n\t"
                                 "scasb	\n\t"
                                 "jne	3f	\n\t"
                                 "testb	%%al,	%%al	\n\t"
                                 "jne	1b	\n\t"
                                 "2:	\n\t"
                                 "xorl	%%eax,	%%eax	\n\t"
                                 "jmp	4f	\n\t"
                                 "3:	\n\t"
                                 "movl	$1,	%%eax	\n\t"
                                 "jl	4f	\n\t"
                                 "negl	%%eax	\n\t"
                                 "4:	\n\t"
            :"=a"(__res)
            :"D"(FirstPart), "S"(SecondPart), "c"(Count)
            :
            );
    return __res;
}

/*

*/

int strlen(char *String) {
    register int __res;
    __asm__ __volatile__    (    "cld	\n\t"
                                 "repne	\n\t"
                                 "scasb	\n\t"
                                 "notl	%0	\n\t"
                                 "decl	%0	\n\t"
            :"=c"(__res)
            :"D"(String), "a"(0), "0"(0xffffffff)
            :
            );
    return __res;
}

/*

*/

unsigned long bit_set(unsigned long *addr, unsigned long nr) {
    return *addr | (1UL << nr);
}

/*

*/

unsigned long bit_get(unsigned long *addr, unsigned long nr) {
    return *addr & (1UL << nr);
}

/*

*/

unsigned long bit_clean(unsigned long *addr, unsigned long nr) {
    return *addr & (~(1UL << nr));
}

/*

*/

unsigned char io_in8(unsigned short port) {
    unsigned char ret = 0;
    __asm__ __volatile__(    "inb	%%dx,	%0	\n\t"
                             "mfence			\n\t"
            :"=a"(ret)
            :"d"(port)
            :"memory");
    return ret;
}

/*

*/

unsigned int io_in32(unsigned short port) {
    unsigned int ret = 0;
    __asm__ __volatile__(    "inl	%%dx,	%0	\n\t"
                             "mfence			\n\t"
            :"=a"(ret)
            :"d"(port)
            :"memory");
    return ret;
}

/*

*/

void io_out8(unsigned short port, unsigned char value) {
    __asm__ __volatile__(    "outb	%0,	%%dx	\n\t"
                             "mfence			\n\t"
            :
            :"a"(value), "d"(port)
            :"memory");
}

/*

*/

void io_out32(unsigned short port, unsigned int value) {
    __asm__ __volatile__(    "outl	%0,	%%dx	\n\t"
                             "mfence			\n\t"
            :
            :"a"(value), "d"(port)
            :"memory");
}

/*

*/

#define port_insw(port, buffer, nr)    \
__asm__ __volatile__("cld;rep;insw;mfence;"::"d"(port),"D"(buffer),"c"(nr):"memory")

#define port_outsw(port, buffer, nr)    \
__asm__ __volatile__("cld;rep;outsw;mfence;"::"d"(port),"S"(buffer),"c"(nr):"memory")

unsigned long rdmsr(unsigned long address) {
    unsigned int tmp0 = 0;
    unsigned int tmp1 = 0;
    __asm__ __volatile__("rdmsr	\n\t":"=d"(tmp0), "=a"(tmp1):"c"(address):"memory");
    return (unsigned long) tmp0 << 32 | tmp1;
}

void wrmsr(unsigned long address, unsigned long value) {
    __asm__ __volatile__("wrmsr	\n\t"::"d"(value >> 32), "a"(value &
                                                                0xffffffff), "c"(address):"memory");
}


#endif

#ifndef _LINKAGE_H_
#define _LINKAGE_H_



#define L1_CACHE_BYTES 32

#define asmlinkage __attribute__((regparm(0)))	

#define ____cacheline_aligned __attribute__((__aligned__(L1_CACHE_BYTES)))

#define SYMBOL_NAME(X)	X

#define SYMBOL_NAME_STR(X)	#X

#define SYMBOL_NAME_LABEL(X) X##:


#define ENTRY(name, handler, has_error_code) \
    .global name; \
name:; \
     xchg %bx,%bx;             \
    .if has_error_code; \
        pushq $0; \
    .endif; \
    pushq %rax; \
    leaq handler(%rip), %rax; \
    xchgq %rax, (%rsp); \
    jmp interrupt_entry;


#endif

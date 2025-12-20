/* raster.s - Assembly routines for rasterization */
/* AT&T syntax for DJGPP/GAS */

    .text
    .globl _hline

/* void hline(int length, fix c1, fix c2, unsigned char *ptr) */
/* Calling convention: cdecl - arguments pushed right-to-left on stack */
/* Stack layout: [ret addr][length][c1][c2][ptr] */
_hline:
    pushl   %ebp
    movl    %esp, %ebp
    pushl   %ebx
    pushl   %edi

    /* Load arguments from stack */
    movl    8(%ebp), %ecx           /* ecx = length */
    movl    12(%ebp), %eax          /* eax = c1 */
    movl    16(%ebp), %edx          /* edx = c2 */
    movl    20(%ebp), %edi          /* edi = ptr */

    /* Calculate xstep = (((c2 - c1) >> 8) + 1) * oneover(length) */
    subl    %eax, %edx              /* edx = c2 - c1 */
    movsbl  %dh, %edx               /* edx = sign-extend((c2-c1) >> 8) - DH contains bits 8-15 */
    incl    %edx                    /* edx = ((c2 - c1) >> 8) + 1 */
    imull   _g_oneOver(,%ecx,4), %edx  /* edx = xstep */
    movl    %edx, %ebx              /* ebx = xstep */

    /* Main loop */
0:
    movb    %ah, (%edi)             /* *ptr = (c1 >> 8) - AH contains bits 8-15 */
    incl    %edi                    /* ptr++ */
    addl    %ebx, %eax              /* c1 += xstep */
    decl    %ecx                    /* length-- */
    jns     0b                      /* loop while length >= 0 */

    /* Restore and return */
    popl    %edi
    popl    %ebx
    popl    %ebp
    ret

; Declare constants for the mutiboot header.
MBALIGN  equ  1 << 0                      ; align loaded modules on page boundaries
MEMINFO  equ  1 << 1                      ; provide memory map
GTVIDEO  equ  1 << 2                      ; provide video mode information
FLAGS    equ  MBALIGN | MEMINFO | GTVIDEO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002                  ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)             ; checksum of above, to prove we are multiboot

; framebuffer
FB_MODE   equ 0      ; 0 mode for linear graphics mode
FB_WIDTH  equ 800    ; screen witdth
FB_HEIGHT equ 600    ; screen height
FB_DEPTH  equ 32 	 ; bits per pixel

 
; Declare a multiboot header that marks the program as a kernel. These are magic
; values that are documented in the multiboot standard. The bootloader will
; search for this signature in the first 8 KiB of the kernel file, aligned at a
; 32-bit boundary. The signature is in its own section so the header can be
; forced to be within the first 8 KiB of the kernel file.
section .multiboot
align 4
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
	dd 0
	dd 0
	dd 0
	dd 0
	dd 0
	dd FB_MODE
	dd FB_WIDTH
	dd FB_HEIGHT
	dd FB_DEPTH
 
; The multiboot standard does not define the value of the stack pointer register
; (esp) and it is up to the kernel to provide a stack. This allocates room for a
; small stack by creating a symbol at the bottom of it, then allocating 16384
; bytes for it, and finally creating a symbol at the top. The stack grows
; downwards on x86. The stack is in its own section so it can be marked nobits,
; which means the kernel file is smaller because it does not contain an
; uninitialized stack. The stack on x86 must be 16-byte aligned according to the
; System V ABI standard and de-facto extensions. The compiler will assume the
; stack is properly aligned and failure to align the stack will result in
; undefined behavior.
section .bss
align 16
kernel_stack_bottom:
resb 16384 ; 16 KiB
kernel_stack_top:

global kernel_stack_bottom
global kernel_stack_top
 
; The linker script specifies _start as the entry point to the kernel and the
; bootloader will jump to this position once the kernel has been loaded. It
; doesn't make sense to return from this function as the bootloader is gone.
; Declare _start as a function symbol with the given symbol size.
section .text

extern kernel_main

global _start:function (_start.end - _start)
_start:
	; set stack pointer
	mov esp, kernel_stack_top

	; Reset EFLAGS (push 0 on stack and pop this value to flags register	)
	push 0
	popf

	; Push the pointer to the Multiboot information structure.
	push ebx
	; Push the magic value.
	push eax
 
	call kernel_main
.end:
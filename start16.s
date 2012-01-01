      # Adapted from:
      # http://wiki.osdev.org/Entering_Long_Mode_Directly
      .equ   START16_SEG, 0x1000
      .text
      .code16
start16:
      movw   $START16_SEG, %ax
      movw   %ax, %ds
      # Save boot drive id
      movl   $bootdrv, %eax   # eax <- Linear address with 20 bits
      movw   %ax, %di
      andw   $0xf, %di        # di <- lowest 4 bits of eax
      shrl   $4, %eax
      movw   %ax, %es         # es <- remaining 16 bits from eax
      movb   %dl, %es:(%di)   # save dl at es:di
      # Enable A20
      inb    $0x92, %al
      orb    $0x02, %al
      andb   $0xfe, %al
      outb   %al, $0x92






# Obter informacao geral VESA
	# VESA Info
	mov %ds, %ax
	mov %ax, %es
	mov $vesa_general_info, %di
	mov $0x4f00, %ax
	int $0x10
	cmp $0x004f, %ax
	je vesa_info_ok
vesa_info_err:
	mov $vesa_info_err_msg, %ax
	call println
	jmp vesa_info_end
vesa_info_ok:
	mov $vesa_info_ok_msg, %ax
	call println
vesa_info_end:

	# Indice %di
	xor %di, %di
	push %di
ciclo:
	# Descobrir ID
	mov $vesa_video_mode_ptr_seg, %bx
	mov (%bx), %ax
	mov $vesa_video_mode_ptr_off, %bx
	mov (%bx), %bx
	pop %di
	add %di, %bx
	push %di
	push %ds
	mov %ax, %ds
	mov (%bx), %cx
	pop %ds
	cmp $0xffff, %cx
	je ciclo_nao_encontrado
	# VESA Mode Info
	mov $video_mode, %bx
	mov %cx, (%bx)
	mov %ds, %ax
	mov %ax, %es
	mov $vesa_mode_info, %di
	# cx = ID
	mov $0x4f01, %ax
	int $0x10
	mov $vesa_mode_info, %bx
	add $18, %bx
	mov (%bx), %ax
	cmp $800, %ax
	jne ciclo_cont
	mov $vesa_mode_info, %bx
	add $20, %bx
	mov (%bx), %ax
	cmp $600, %ax
	jne ciclo_cont
	mov $vesa_mode_info, %bx
	add $25, %bx
	mov (%bx), %al
	cmp $16, %al
	jne ciclo_cont
	jmp ciclo_encontrado
ciclo_cont:
	pop %di
	add $2, %di
	push %di
	jmp ciclo
ciclo_encontrado:
	mov $vesa_mode_info_ok_msg, %ax
	call println
	jmp ciclo_out
ciclo_nao_encontrado:
	mov $vesa_mode_info_err_msg, %ax
	call println
	jmp ciclo_out
ciclo_out:

	# Set the system into graphics mode, by using BIOS service INT 0x10, with AX=0x4f02 and
	# BX set to the video mode id for 640x480 or 800x600 at 16bpp. 
	mov $0x4f02, %ax
	mov $video_mode, %bx
	mov (%bx), %bx
	orw  $0x4000, %bx
	int $0x10






      # Build page tables
      xorw   %bx, %bx
      movw   %bx, %es
      cld
      movw   $0xa000, %di
      movw   $0xb00f, %ax
      stosw
      xorw   %ax, %ax
      movw   $0x07ff, %cx
      rep    stosw
      movw   $0xc00f, %ax
      stosw
      xorw   %ax, %ax
      movw   $0x07ff, %cx
      rep    stosw
      movw   $0x018f, %ax
      stosw
      xorw   %ax, %ax
      movw   $0x07ff, %cx
      rep    stosw
      # Enter long mode
      cli                       # No IDT. Keep interrupts disabled.
      movl   $0xA0, %eax        # Set PAE and PGE
      movl   %eax, %cr4
      movl   $0x0000a000, %edx  # Point CR3 at PML4
      movl   %edx, %cr3
      movl   $0xC0000080, %ecx  # Specify EFER MSR
      rdmsr
      orl    $0x00000100, %eax  # Enable long mode
      wrmsr
      movl   %cr0, %ebx
      orl    $0x80000001, %ebx  # Activate long mode
      movl   %ebx, %cr0         # by enabling paging and protection simultaneously
      lgdtl  gdt_ptr            # Set Global Descriptor Table
      ljmpl  $1<<3, $start64    # Jump to 64-bit code start
      .align 16
gdt:
      .quad  0x0000000000000000
      .quad  0x0020980000000000
      .quad  0x0000900000000000

gdt_ptr:
      .word  (gdt_ptr-gdt-1)
      .long  (START16_SEG*16+gdt)
      .long  0
.global teste
teste:
	.word 0xA5
	



vesa_info_err_msg:
.asciz "VESA Info Error"
vesa_info_ok_msg:
.asciz "VESA Info OK"
vesa_mode_info_err_msg:
.asciz "VESA Mode Info Not Found"
vesa_mode_info_ok_msg:
.asciz "VESA Mode Info OK"



.global vesa_general_info_mem
vesa_general_info_mem:
vesa_general_info:
vesa_signature:
.byte 'V'
.byte 'B'
.byte 'E'
.byte '2'
vesa_version:
.word 0
vesa_vendor_string:
.word 0
.word 0
vesa_capabilities:
.word 0
.word 0
vesa_video_mode_ptr:
vesa_video_mode_ptr_off:
.word 0
vesa_video_mode_ptr_seg:
.word 0
vesa_total_memory:
.word 0
vesa_reserved:
.space 750


.global video_mode
video_mode:
.word 0
vesa_mode_info:
.space 128
.space 128



	nop
	nop
	nop
	nop
	nop
	

put_char: # recebe o caracter em %al
	# escrita a posicao
	# xorw %si, %si
	# movw $0xB800, %bx
	# movw %bx, %es
	# movw $0x2, %bx
	# movw $0xAA50, %es:(%bx, %si)
	mov $7, %bl # cor cinzenta no preto
	xorb %bh, %bh # pagina 0
	movb $0x0E, %ah # identifica o servico
	int $0x10 # chamada do servico BIOS atraves de interrupcao
	ret
	
print: # escreve string terminada em 0, endereço em %ax
	movw %ax, %di
ciclo_escrita:
	mov %di, %bx
	movb (%bx), %al
	inc %di
	cmpb $0, %al
	jz ciclo_escrita_out
	call put_char
	jmp ciclo_escrita
ciclo_escrita_out:
	ret
	
new_line: # escreve uma mudança de linha e carriage return
	movb $'\n', %al
	call put_char
	movb $'\r', %al
	call put_char
	ret
	
println: # escreve uma string e muda de linha
	call print
	call new_line
	ret

.end

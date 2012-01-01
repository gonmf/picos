      .text
      .code64
      .global start64
start64:
	# Fix segment registers
	movw  $0x10, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %es # 0x105e6, breaks
      # Clear BSS
      movl   $__bss_start, %edi
      movl   $__bss_quads, %ecx
      xorq   %rax, %rax
      cld
      rep    stosq

      movl   $0x10000, %esp
      call   frame_main

1:    hlt
      jmp 1b

      .global bootdrv
bootdrv:
      .byte 0


timer_leitura: # le o valor do contador para o rax
	xor %rax, %rax
	mov $0x00, %al
	outb %al, $0x43 # Latch
	inb $0x40
	xchg %al, %ah
	inb $0x40
	xchg %al, %ah
	ret
timer_escrita: # escreve o valor do rax para o contador
	xchg %rax, %rcx
	mov $0x30, %al # conta para baixo e reinicia apos 0
	outb %al, $0x43 # Controlo
	xchg %rax, %rcx
	outb %al, $0x40 # Low byte
	xchg %al, %ah
	outb %al, $0x40 # High byte
	ret
delay10ms:
	mov $0x2e98, %ax # 11927,37 para cima
	call timer_escrita
1:
	call timer_leitura
	cmp $0, %ax
	jg 1b

	ret
.global delay
delay: # recebe numero de dezenas de millisegundos em rax
	push %rax
	mov %rdi, %rax
	push %rcx
1:
	push %rax
	call delay10ms
	pop %rax
	sub $1, %rax
	jnz 1b
	pop %rcx
	pop %rax
	ret
.end

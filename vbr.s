.equ START_ADDR, 0x7C00
.section .text
.code16
	cli # Desactivar interrupcoes
	ljmp $0, $norm_cs
norm_cs:
	xorw %ax, %ax
	movw %ax, %ds
	# cuidado em nao mudar o cs code segment
	movw %ax, %ss
	movw %ax, %es
	movw $0x7C00, %sp # stack cresce para baixo
	sti # Activar interrupcoes
	movw $drive_id, %bx # guardar o ID da drive para ser usada
	movb %dl, (%bx)     # em chamadas INT 13h AH=42h

########	Convencao de chamadas:
# main #	Chamador guarda o que nao pode perder, chamado nao guarda nada
########	Parametros passados por ordem em registos, eax, ebx, ...

leitura_bloco:
	movw $10, %ax
	call read_disk_by_block
	movw $bloco_lido, %bx
	add $24, %bx
	movw (%bx), %ax
	call read_disk_by_block
	movw $bloco_lido, %ax
	movw $7, %bx
ciclo_directoria: # ax contem ref p/ inode, ax+2 nome de ficheiro
	push %bx
	push %ax
	add $2, %ax
	mov $sys_code_file, %bx
	call strcmp # vai comparar ax com bx, retorna 0 se iguais
	mov $0, %cx
	cmp %ax, %cx
	je ciclo_continue
	pop %ax
	add $32, %ax
	pop %bx
	sub $1, %bx
	jnz ciclo_directoria
	mov $not_found, %ax
	call print
	jmp stop
ciclo_continue:
	pop %bx # ordem ao contrario intencional
	pop %ax
	# calculo do sector do i_node a carregar: 20 + i_node / 8 (div inteira) (em sectores)
	mov (%bx), %ax
	mov %ax, %bx
	# ax contem numero do inode do ficheiro
	shr $3, %ax
	add $20, %ax
	push %bx
	call read_disk_by_sector
	pop %bx
	# shift para chegar ao bloco certo no sector lido: offset = inode % 8 (resto da div inteira) (em bytes)
	# %bx tem o numero do inode
resto_divisao_inteira:
	cmp $8, %bx
	jle resto_divisao_inteira_out
	sub $8, %bx
	jmp resto_divisao_inteira
resto_divisao_inteira_out:
	mov $0, %ax
multiplicacao_do_resto:
	cmp $1, %bx
	je multiplicacao_do_resto_out
	add $64, %ax
	sub $1, %bx
	jmp multiplicacao_do_resto
multiplicacao_do_resto_out:
	mov %ax, %bx
	movw $bloco_lido, %ax
	# %ax aponta para inicio do bloco lido
	# %bx tem o offset (bytes) para chegar ao i_node pretendido
	add %ax, %bx
	add $24, %bx # devia ser 26, o que raio se passa aqui?!
	mov $0, %cx
	mov $0, %dx
	# bx contem o inicio em memoria do sector lido mesmo em cima da inf. de inode da zona de dados
ciclo_de_varias_zonas:
	add $1, %dx
	push %dx
	mov (%bx), %ax
	cmp $0, %ax
	je ciclo_de_varias_zonas_out
########################################################
# Stub de primeira indirecao
# ax = zona (9xx) que contem uma serie de inodes (4 bytes)
# bx = ponteiro para a posicao em memoria da zona
	cmp $8, %dx
	jne ciclo_de_varias_zonas_cont
	mov $dap_destsegment, %bx
	movw $0, (%bx)
	mov $bloco_lido, %dx
	mov $dap_destoffset, %bx
	movw %dx, (%bx)
	call read_disk_by_block
	mov $bloco_lido, %bx
	mov (%bx), %ax
	cmp $0, %ax
	je ciclo_de_varias_zonas_out
ciclo_de_varias_zonas_cont:
########################################################
	push %bx
	mov $dap_numsectors, %bx # fixo em 2 sectors = 1 zona
	movb $2, (%bx)
	movw $segment, %bx
	movw (%bx), %si
	mov $dap_destsegment, %bx # segmento fixo para menos de 64k de dados
	movw %si, (%bx)
	mov $dap_destoffset, %bx
	movw %cx, (%bx)
	call read_disk_by_block
	pop %bx
	add $4, %bx
	add $1024, %cx
	jnz ciclo_de_varias_zonas_cont2
	movw $segment, %bx
	movw (%bx), %si
	add $1, %si
	movw %si, (%bx)
ciclo_de_varias_zonas_cont2:
	pop %dx
	jmp ciclo_de_varias_zonas
ciclo_de_varias_zonas_out:
	mov $drive_id, %bx
	movb (%bx), %dl # passagem de drive id
	ljmp $0x1000, $0 # long jump
	jmp stop
stop:
	hlt
	jmp stop
	nop
	nop
	nop
	nop
	nop






###########
# Funcoes #
###########

strcmp: # compara duas strings acabadas em \0, começadas em ax e bx, retorna 1 se diferentes, 0 se iguais
	mov %ax, %bp
	movb (%bp), %cl
	movb (%bx), %dl
	cmp %cx, %dx
	jne return_strcmp_no_equal
	mov $0, %bp
	sub %bp, %cx
	jz return_strcmp_equal
	sub %bp, %dx
	jz return_strcmp_equal
	add $1, %ax
	add $1, %bx
jmp strcmp
return_strcmp_equal:
	mov $0, %ax
	ret
return_strcmp_no_equal:
	mov $1, %ax
	ret

read_disk_by_block: # recebe sector a ler em %ax, em blocos de 1024 bytes, incrementa base partitionoffset
	shl $1, %ax
	call read_disk_by_sector
	ret

read_disk_by_sector: # recebe sector a ler em %ax, em sectores de 512 bytes, incrementa base partitionoffset
	movw $partitionoffset, %bx
	movw (%bx), %bx
	add %bx, %ax
	movw $dap_lbafirstsector, %bx
	movw %ax, (%bx) #  sector a ler: 43364 para o inode da root
	movw $drive_id, %bx
	movb (%bx), %dl
	movb $0x42, %ah # service identifier
	movw $dap, %si # offset of DAPs memory location
	int $0x13 # chamada
	ret

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











##############
# Constantes #
##############

.section .rodata
not_found:
.asciz "System file not found"
sys_code_file:
.asciz "frame.sys"














#############
# Variaveis #
#############

.section .data
partitionoffset:
.word 43344 # sectores (512 bytes)
dap:
.byte 16
.byte 0
dap_numsectors:
.byte 1
.byte 0
dap_destoffset:
.word bloco_lido
dap_destsegment:
.word 0
dap_lbafirstsector:
.word 0
.word 0
.word 0
.word 0
.word 0
.word 0
.word 0
.word 0
drive_id:
.byte 0
segment:
.word 0x1000
bloco_lido:
.end

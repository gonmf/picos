#include "common.h"
/*
 * Compara duas strings, retorna true se iguais;
 *
 * NAO CONFUNDIR COM strcompare(..) ABAIXO
 */
bool strcmp(char * a, char * b){
	while(*a){
		if(!(*b))
			return false;
		if(*a != *b)
			return false;
		a++;
		b++;
	}
	return *a == *b;
}
/*
 * Evitar usar malloc; Implementacao MUITO incompleta;
 */
#define mallocmem (1024*2)
char mem[mallocmem]; // 2 KiB
d32 free_mem = 0;
char * malloc(d32 size){
	if(size<1)
		print("Illegal malloc size, minimum 1 byte request.");
	if(mallocmem<free_mem + size)
		print("Illegal malloc size, maximum 2KiBs total.");
	int ret = free_mem;
	free_mem += size;
	return mem + ret;
}
u64 strlen(char * s){
	u64 ret = 0;
	while(*s != 0){
		s++;
		ret++;
	}
	return ret;	
}
/* corrigir *
char * strtrim(char * s){
	u64 l = 0;
	u64 r = strlen(s);
	u64 i = 0;
	char * st = s;
	while(*st == '0' && *st != 0){
		st++;
		l++;
	}
	st = s + r - 1;
	while(*st == '0' && *st != 0){
		st--;
		r--;
	}
	char * ret = malloc(r - l + 2);
	st = s + l;
	i = l;
	while(i < r){
		*ret = *st;
		st++;
		ret++;
		i++;
	}
	return ret;
}*/
char nibble_to_asciihex(char n){
	n = n & 0xf;
	if(n < 10)
		return n + '0';
	n -= 10;
	return n + 'a';
}
char * strtrimleft(char * s){
	while(*s == ' '){
		s++;
	}
	return s;
}
/*
 * Funcoes de cast explicito
 * 
 * Transformam inteiros sem sinal em representacoes decimais e hexadecimais
 *
 * EVITAR USAR; CHAMAM MALLOC
 */
char * typecastD8(u8 i){
	char * ret = malloc(4);
	u64 p = 4;
	while(--p)
		*(ret+p) = ' ';
	*(ret) = ' ';
	*(ret+3) = 0;	
	p = 3;
	while(i){
		*(ret+(--p)) = '0' + (i % 10);
		i = i / 10;
	}
	return strtrimleft(ret);
}
char * typecastD16(u16 i){
	char * ret = malloc(6);
	u64 p = 6;
	while(--p)
		*(ret+p) = ' ';
	*(ret) = ' ';
	*(ret+5) = 0;	
	p = 5;
	while(i){
		*(ret+(--p)) = '0' + (i % 10);
		i = i / 10;
	}
	return strtrimleft(ret);
}
char * typecastD32(u32 i){
	char * ret = malloc(11);
	u64 p = 11;
	while(--p)
		*(ret+p) = ' ';
	*(ret) = ' ';
	*(ret+10) = 0;	
	p = 10;
	while(i){
		*(ret+(--p)) = '0' + (i % 10);
		i = i / 10;
	}
	return strtrimleft(ret);
}
char * typecastD64(u64 i){
	char * ret = malloc(20);
	u64 p = 20;
	while(--p)
		*(ret+p) = ' ';
	*(ret) = ' ';
	*(ret+19) = 0;	
	p = 19;
	while(i){
		*(ret+(--p)) = '0' + (i % 10);
		i = i / 10;
	}
	return strtrimleft(ret);
}
char * typecastU8(u8 i){
	char * ret = malloc(5);
	*(ret+0) = '0';
	*(ret+1) = 'x';
	*(ret+2) = nibble_to_asciihex(i >> 4);
	*(ret+3) = nibble_to_asciihex(i >> 0);
	*(ret+4) = 0;
	return ret;
}
char * typecastU16(u16 i){
	char * ret = malloc(7);
	*(ret+0) = '0';
	*(ret+1) = 'x';
	*(ret+2) = nibble_to_asciihex(i >> 12);
	*(ret+3) = nibble_to_asciihex(i >> 8);
	*(ret+4) = nibble_to_asciihex(i >> 4);
	*(ret+5) = nibble_to_asciihex(i >> 0);
	*(ret+6) = 0;
	return ret;
}
char * typecastU32(u32 i){
	char * ret = malloc(11);
	*(ret+0) = '0';
	*(ret+1) = 'x';
	*(ret+2) = nibble_to_asciihex(i >> 28);
	*(ret+3) = nibble_to_asciihex(i >> 24);
	*(ret+4) = nibble_to_asciihex(i >> 20);
	*(ret+5) = nibble_to_asciihex(i >> 16);
	*(ret+6) = nibble_to_asciihex(i >> 12);
	*(ret+7) = nibble_to_asciihex(i >> 8);
	*(ret+8) = nibble_to_asciihex(i >> 4);
	*(ret+9) = nibble_to_asciihex(i >> 0);
	*(ret+10) = 0;
	return ret;
}
char * typecastU64(u64 i){
	char * ret = malloc(19);
	*(ret+0) = '0';
	*(ret+1) = 'x';
	*(ret+2) = nibble_to_asciihex(i >> 58);
	*(ret+3) = nibble_to_asciihex(i >> 54);
	*(ret+4) = nibble_to_asciihex(i >> 52);
	*(ret+5) = nibble_to_asciihex(i >> 48);
	*(ret+6) = nibble_to_asciihex(i >> 44);
	*(ret+7) = nibble_to_asciihex(i >> 40);
	*(ret+8) = nibble_to_asciihex(i >> 36);
	*(ret+9) = nibble_to_asciihex(i >> 32);
	*(ret+10) = nibble_to_asciihex(i >> 28);
	*(ret+11) = nibble_to_asciihex(i >> 24);
	*(ret+12) = nibble_to_asciihex(i >> 20);
	*(ret+13) = nibble_to_asciihex(i >> 16);
	*(ret+14) = nibble_to_asciihex(i >> 12);
	*(ret+15) = nibble_to_asciihex(i >> 8);
	*(ret+16) = nibble_to_asciihex(i >> 4);
	*(ret+17) = nibble_to_asciihex(i >> 0);
	*(ret+18) = 0;
	return ret;
}
/*
 * Aloca espaco e copia uma string
 *
 * EVITAR USAR; CHAMA MALLOC
 */
char * strclone(char * s){
	char * r = malloc(strlen(s) + 1);
	char * rt = r;
	while(*s != 0){
		*r = *s;
		s++;
		r++;
	}
	*r = 0;
	return rt;
}
/*
 * Aloca espaco e concatena duas strings
 *
 * EVITAR USAR; CHAMA MALLOC
 */
char * strconcat(char * s1, char * s2){
	char * r = malloc(strlen(s1) + strlen(s2) + 1);
	char * rt = r;
	while(*s1 != 0){
		*r = *s1;
		s1++;
		r++;
	}
	while(*s2 != 0){
		*r = *s2;
		s2++;
		r++;
	}
	return rt;
}
/*
 * Compara duas strings, retorna 0 se iguais, outro valor se diferentes;
 *
 * NAO CONFUNDIR COM strcmp(..) ACIMA
 */
u64 strcompare(char * s1, char * s2){
	while(*s1 == *s2){
		if(*s1 == 0)
			return 0;
		s1++;
		s2++;
	}
	return *s1 - *s2;
}
/*
 * Identificaa posicao de uma string num array de strings
 */
u64 strindexof(char * hay[], char * needle){
	u64 index = 0;
	while(*hay != 0){
		if(strcompare(hay[index], needle) == 0)
			return index;
		++index;
	}
	return -1;
}

void print(char * s){
	while(*s != 0){
		putchar(*s);
		++s;
	}
}
void printf(char * s, char * args){
	while(*s != 0){
		if(*s == '%')
			print(args);
		else		
			putchar(*s);
		++s;
	}
}

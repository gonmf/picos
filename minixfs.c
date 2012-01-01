#include "common.h"
/*
 * Preenche Inode de ficheiro ou directoria
 */
MINIX_INODE buffer_inode[512/sizeof(MINIX_INODE)]; // 512 bytes
u32 open_inode(PARTITION * part, u32 inode, MINIX_INODE * dest){
	--inode; // inodes comecam do 1 em vez de 0
	u32 sector = MINIX_OFFSET_TO_INODES_IN_SECTORS + inode / 8;
	u32 offset = (inode % 8);
	buffer_inode[offset].i_zone[0] == 0x0;
	u32 errno = read_sectors(part, sector, 1, buffer_inode);
	(*dest) = buffer_inode[offset];
	if(buffer_inode[offset].i_zone[0] == 0x7890)
		return 0x9870;
	return errno;
}
/*
 * Recebe o Inode e preenche um array de zonas
 * Retorna -1 em caso de erro ou o numero de zonas lidas
 * Não distingue Inodes de ficheiros de directorias
 *
 * RETORNO
 * -1 em caso de erro
 * outro valor (positivo) em caso de sucesso, o num de zonas lidas em array
 *
 * PROBLEMAS
 * So le ate um maximo de 7 + 1024 + 1024 * 1024 = 1049607 zonas,
 * ou seja pouco mais de um GiB.
 */
ZONE buff[1024/sizeof(ZONE)]; // 1 KiB
ZONE buff2[1024/sizeof(ZONE)]; // 1 KiB
MINIX_INODE _inode;
u32 read_zones(PARTITION * part, u32 inode, ZONE * dest){
	open_inode(part, inode, &_inode);
	ZONE * buffer = _inode.i_zone;
	u64 i=0;
	u64 j=0;
	u64 k=0;
	u64 l=0;
	while(i < 10){
		if(buffer[i] == 0){
			dest[k] = 0;
			return k;
		}
		++i;
		if(i<8)
			dest[k++] = buffer[i-1];
		if(i==8){
			j = 0;
			read_sectors(part, buffer[7] * 2, 2, buff);
			while((j<1024/sizeof(ZONE)) && (buff[j] != 0))
				dest[k++] = buff[j++];
		}
		if(i==9){
			j = 0;
			read_sectors(part, buffer[8] * 2, 2, buff);
			while((j<1024/sizeof(ZONE)) && (buff[j] != 0)){
				l = 0;
				read_sectors(part, buff[j++] * 2, 2, buff2);
				while((l<1024/sizeof(ZONE)) && (buff2[l] != 0))
					dest[k++] = buff2[l++];
			}
		}
		if(i==10) /* Nao suportado */
			return -1;
	}
	dest[k] = 0;
	return k;
}
/*
 * Preenche um array com o conteudo de um array de zonas
 * Nao distingue ficheiros de directorias
 *
 * RETORNO
 * 0x0 em caso de sucesso
 * outro valor positivo em caso de erro
 */
u32 build_buffer(PARTITION * part, ZONE * src, u32 len /* num de zonas */, void * dest){
	u32 i = 0;
	u32 tst = 0;
	while(i < len){
		if((tst = read_sectors(part, src[i] * 2, 2, dest + i * 1024)) > 0)
			return tst;
		++i;
	}
	return 0x0;
}
/*
 * Compara nomes ate encontrar a directoria ou ficheiro
 *
 * RETORNO
 * 0x0 em caso de ficheiro nao encontrado
 * outro valor positivo com o valor do inode
 */
u64 find_inode_entry(char * needle, MINIX_DIR_ENTRY * entries, u64 num_entries /* ou -1 */){
	u64 i = 0;
	while(i < num_entries){
		if(entries[i].inode == 0)
			return 0;
		if(strcompare(entries[i].name, needle) == 0) // nao confundir com o strcmp
			return entries[i].inode;
		++i;
	}
	return 0;
}

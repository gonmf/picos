#include "common.h"
static inline void outb(u8 v, u16 port){ // Linux source
	asm volatile("outb %0,%1" : : "a" (v), "dN" (port));
}
static inline u8 inb(u16 port){ // Linux source
	u8 v;
	asm volatile("inb %1,%0" : "=a" (v) : "dN" (port));
	return v;
}

static inline void rep_insw(u16 port, void * dest, u64 n){
	asm volatile(
		"cld; rep insw"
		:"=D"(dest), "=c"(n)
		:"d"(port), "D"(dest), "c"(n)
	);
}
#define DataRegister 0x01f0
#define ErrorRegister 0x01f1
#define FeaturesRegister 0x01f1
#define SectorCountRegister 0x01f2
#define LBA7_0Register 0x01f3
#define LBA15_8Register 0x01f4
#define LBA23_16Register 0x01f5
#define DriveLBA27_24Register 0x01f6
#define StatusRegister 0x01f7
#define CommandRegister 0x01f7
#define AltStatusRegister 0x03f6
#define DeviceCtrlRegister 0x03f6
#define BSY 0x80
#define RDY 0x40
#define DF 0x20
#define DRQ 0x08
#define ERR 0x01
#define nIEN 0x02
#define SRST 0x04
bool init = false;
void init_ata(){
	if(init)
		return;
	while(inb(StatusRegister) & BSY); // Wait for BSY = 0
	outb(nIEN, DeviceCtrlRegister); // Desactivar interrupcoes
	init = true;
}
/* RETORNO
 * 0x0 se bem sucedido
 * 0x1 erro disco
 * 0x2 erro desconhecido
 * 0x3 erro input
 */
u32 read_sectors_bare(u32 sector, u8 nsects, void * dest){
	init_ata();
	if(nsects == 0)
		return 0;
	if(nsects < 0 || sector < 0)
		return 3;
	u8 u8aux = 0x00;
	while(inb(StatusRegister) & BSY); // Wait for BSY = 0
	outb(0xe0 | (sector >> 24), DriveLBA27_24Register); // LBA mode, Master, 27:24 LBA addr
	inb(StatusRegister); // Wait 400ns
	inb(StatusRegister);
	inb(StatusRegister);
	inb(StatusRegister);
	outb((sector >> 16) & 0xff, LBA23_16Register); // LBA addr
	outb((sector >> 8) & 0xff, LBA15_8Register); // LBA addr
	outb(sector & 0xff, LBA7_0Register); // LBA addr
	outb(nsects, SectorCountRegister); // num sectors
	outb(0, FeaturesRegister); // PIO mode
	outb(0x20, CommandRegister); // Read Sectors
	inb(StatusRegister); // Wait 400ns
	inb(StatusRegister);
	inb(StatusRegister);
	inb(StatusRegister);
	do{
		while((u8aux = inb(StatusRegister)) & BSY); // Wait for BSY = 0
		while(!((u8aux = inb(StatusRegister)) & (ERR | DF | DRQ))); // Wait for ERR | DF | DRQ
		if(u8aux & (ERR | DF)) // Erro declarado
			return 1;
		if(!(u8aux & DRQ)) // Erro nao declarado
			return 2;
		void * tmpdest = dest;
		rep_insw(DataRegister, dest, 512 / sizeof(u16)); // Ler um sector
		dest = (tmpdest += 512);
	}while(--nsects);
	return 0;
}
/* RETORNO
 * 0x0 se bem sucedido
 * 0x1 erro disco
 * 0x2 erro desconhecido
 * 0x3 erro input
 */
u32 read_sectors(PARTITION * part, u32 sector, u8 nsects, void * dest){
	return read_sectors_bare(sector + part->start, nsects, dest);
}
/* RETORNO
 * 0x0 sucesso
 * 0x1 erro
 */
u8 buffer[512];
u32 read_partition(PARTITION * part, u32 npart /* num de particao comeca em 0 */){
	read_sectors_bare(0 /* primeiro LBA do disco */, 1, buffer);
	if((buffer[446 + 0 + 16 * npart]) == 0x80) // active partition
		part->active = 1;
	else if((buffer[446 + 0 + 16 * npart]) == 0x00)
		part->active = 0;
	else{
		part->active = buffer[446 + 0 + 16 * npart];
		return 1;
	}
	part->start = ((PART_ENTRY *)(buffer + 446))[npart].lba_start;
	part->length = ((PART_ENTRY *)(buffer + 446))[npart].lba_count;
	return 0;
}

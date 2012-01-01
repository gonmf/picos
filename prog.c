#include "common.h"
#define BASE_CODE_VALUE 0x10000
extern char vesa_general_info_mem[];
pt_vesa_general_info vesa_info = (pt_vesa_general_info)(vesa_general_info_mem + BASE_CODE_VALUE);
extern char video_mode[];
pt_mode_w_id vesa_mode = (pt_mode_w_id)(video_mode + BASE_CODE_VALUE);
extern void delay(u64 value);
typedef struct _page_table{
	u64 entry[512];
} ty_page_table, * pt_page_table;
MINIX_DIR_ENTRY dir_entries[1024/sizeof(MINIX_DIR_ENTRY)]; // 8 KiB, 256 entradas em directoria
ZONE zones[2*1024*sizeof(ZONE)]; // 8 KiB, 2000 zonas
void paginacao(){
	pt_page_table pml4 = (pt_page_table)(0xa000);
	pt_page_table pdpt = (pt_page_table)((pml4->entry[0]) & 0xfffff000);
	pdpt->entry[0x03] = 0xd00f;
	pt_page_table pd = (pt_page_table)(0xc000);
	pd->entry[0x1] = 0x00200000 | 0x0000018f;
	pd = (pt_page_table)(0xd000);
	pd->entry[0x100] = (vesa_mode->mode.lfb_ptr & 0xffe00000) | 0x0000018f;
}
char * strtf = "img*.bmp";
void frame_main(){
	paginacao();
	void * gfx_buffer = (void *)0xe0000000;
	void * img_buffer = (void *)0x00100000;
	PARTITION partition;
	read_partition(&partition, PARTITION_NUM); // Saber particao MINIX
	build_buffer(&partition, zones, read_zones(&partition, MINIX_ROOT_INODE, zones), dir_entries); // Descobrir directoria root
	u32 inode_pasta_fotos = find_inode_entry("fotos", dir_entries, -1); // Descobrir pasta /fotos

	u64 NUM_FOTOS = 3;
	u64 TEMPO_SEG = 5;
	u64 IDX_FOTOS = 0;
	while(1){
		strtf[3] = '0' + IDX_FOTOS;
		build_buffer(&partition, zones, read_zones(&partition, inode_pasta_fotos, zones), dir_entries);
		build_buffer(&partition, zones, read_zones(&partition, find_inode_entry(strtf, dir_entries, -1), zones), img_buffer);
		draw_bmp(img_buffer);
		delay(TEMPO_SEG * 100);
		++IDX_FOTOS;
		if(IDX_FOTOS == NUM_FOTOS)
			IDX_FOTOS = 0;
	}
}

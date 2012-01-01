typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned int u32;
typedef unsigned long int u64;
typedef char d8;
typedef short int d16;
typedef int d32;
typedef long int d64;
#define true 1
#define false 0
typedef d32 bool;
typedef struct{
	u16 off, seg;
} far_ptr;
/* numero de particao comeca no 0 */
#define PARTITION_NUM 1
/* offset em numero de sectores do inicio da particao ate lista de inodes */
#define MINIX_OFFSET_TO_INODES_IN_SECTORS 20
/* numero de primeiro inode (root) em filesystem MINIX */
#define MINIX_ROOT_INODE 1
typedef u32 LBA;
typedef char * DISK;
typedef struct _PART_ENTRY{
	u32 __;
	u32 ___;
	u32 lba_start;
	u32 lba_count;
} PART_ENTRY;
typedef struct _PARTITION{
	char active;
	u32 start;
	u32 length;
} PARTITION;
typedef struct minix_dir_entry {
	u16 inode;
	char name[30];
} MINIX_DIR_ENTRY;
typedef u32 ZONE;
typedef struct minix_super_block{
	u16 s_ninodes;
	u16 s_nzones;
	u16 s_imap_blocks;
	u16 s_zmap_blocks;
	u16 s_firstdatazone;
	u16 s_log_zone_size;
	u32 s_max_size;
	u16 s_magic;
	u16 s_state;
	u32 s_zones;
} MINIX_SUPER_BLOCK;
typedef struct minix2_inode{
	u16 i_mode;
	u16 i_nlinks;
	u16 i_uid;
	u16 i_gid;
	u32 i_size;
	u32 i_atime;
	u32 i_mtime;
	u32 i_ctime;
	u32 i_zone[10];
} MINIX_INODE;
typedef struct st_vesa_mode_info{
	u16 mode_attr;          /* 0 */
	u8 win_attr[2];         /* 2 */
	u16 win_grain;          /* 4 */
	u16 win_size;           /* 6 */
	u16 win_seg[2];         /* 8 */
	far_ptr win_scheme;     /* 12 */
	u16 logical_scan;       /* 16 */
	u16 h_res;              /* 18 */
	u16 v_res;              /* 20 */
	u8 char_width;          /* 22 */
	u8 char_height;         /* 23 */
	u8 memory_planes;       /* 24 */
	u8 bpp;                 /* 25 */
	u8 banks;               /* 26 */
	u8 memory_layout;       /* 27 */
	u8 bank_size;           /* 28 */
	u8 image_planes;        /* 29 */
	u8 page_function;       /* 30 */
	u8 rmask;               /* 31 */
	u8 rpos;                /* 32 */
	u8 gmask;               /* 33 */
	u8 gpos;                /* 34 */
	u8 bmask;               /* 35 */
	u8 bpos;                /* 36 */
	u8 resv_mask;           /* 37 */
	u8 resv_pos;            /* 38 */
	u8 dcm_info;            /* 39 */
	u32 lfb_ptr;            /* 40 Linear frame buffer address */
	u32 offscreen_ptr;      /* 44 Offscreen memory address */
	u16 offscreen_size;     /* 48 */
	u8 reserved[206];       /* 50 */
} ty_vesa_mode_info, * pt_vesa_mode_info;
typedef struct st_mode_w_id{
	u16 mode_id;
	ty_vesa_mode_info mode;
} __attribute__((packed)) ty_mode_w_id, * pt_mode_w_id;
typedef struct st_vesa_general_info{
	u32 signature;          /* 0 Magic number = "VESA" */
	u16 version;            /* 4 */
	far_ptr vendor_string;  /* 6 */
	u32 capabilities;       /* 10 */
	far_ptr video_mode_ptr; /* 14 */
	u16 total_memory;       /* 18 */
	u8 reserved[236];       /* 20 */
} ty_vesa_general_info, * pt_vesa_general_info;

typedef struct st_rgb{
	unsigned : 3;
	unsigned b:5;
	unsigned : 2;
	unsigned g:6;
	unsigned : 3;
	unsigned r:5;
} __attribute__((packed)) RGB;
typedef struct st_rgb16{
	unsigned b:5;
	unsigned g:6;
	unsigned r:5;
} __attribute__((packed)) RGB16;
typedef struct st_bitmapfileheader{
	u16 type; // BM
	u32 size;
	u16 __;
	u16 ___;
	u32 data; // offset from the beginning of the file to the bitmap data
} __attribute__((packed)) ty_bitmapfileheader, * pt_bitmapfileheader;

// device driver
static inline void outb(u8 v, u16 port);
static inline u8 inb(u16 port);
static inline void rep_insw(u16 port, void * dest, u64 n);
u32 read_sectors_bare(u32 sector, u8 nsects, void * dest);
u32 read_sectors(PARTITION * part, u32 sector, u8 nsects, void * dest);
u32 read_partition(PARTITION * part, u32 npart /* num de particao comeca em 0 */);
// minix filesystem
u32 open_inode(PARTITION * part, u32 inode, MINIX_INODE * dest);
u32 read_zones(PARTITION * part, u32 inode, ZONE * dest);
u32 build_buffer(PARTITION * part, ZONE * src, u32 len, void * dest);
u64 find_inode_entry(char * needle, MINIX_DIR_ENTRY * entries, u64 num_entries /* ou -1 */);
// display
void draw_bmp(void * img_buffer);
void gfx_clrsrc(u32 dsp);
void gfx_demo(u32 dsp);
void scroll();
void putchar(char c);
void gotoXY(d32 x, d32 y);
void clrsrc();
// string
bool strcmp(char * a, char * b);
char * malloc(d32 size);
u64 strlen(char * s);
char * strtrim(char * s);
char nibble_to_asciihex(char n);
char * strtrimleft(char * s);
char * typecastD8(u8 i);
char * typecastD16(u16 i);
char * typecastD32(u32 i);
char * typecastD64(u64 i);
char * typecastU8(u8 i);
char * typecastU16(u16 i);
char * typecastU32(u32 i);
char * typecastU64(u64 i);
char * strclone(char * s);
char * strconcat(char * s1, char * s2);
u64 strcompare(char * s1, char * s2);
u64 strindexof(char * hay[], char * needle);
void print(char * s);
void printf(char * s, char * args);

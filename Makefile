CFLAGS = -fno-stack-protector -m64 -ffreestanding -mno-red-zone -O2

all: disks/frame-hd63-flat.img frame.sys

disks/frame-hd63-flat.img: vbr.sys
	dd if=vbr.sys of=disks/frame-hd63-flat.img bs=512 seek=43344 count=1 conv=notrunc

vbr.sys: vbr.o
	ld -T vbr.ld -o vbr.sys vbr.o

vbr.o: vbr.s
	as -o vbr.o vbr.s

frame.sys: start16.o start64.o devicedriver.o minixfs.o display.o string.o prog.o
	ld -M -T prog.ld -o frame.sys start16.o start64.o devicedriver.o minixfs.o display.o string.o prog.o > frame.map

devicedriver.o: devicedriver.c common.h
	gcc $(CFLAGS) -c -o devicedriver.o devicedriver.c

minixfs.o: minixfs.c common.h
	gcc $(CFLAGS) -c -o minixfs.o minixfs.c

display.o: display.c common.h
	gcc $(CFLAGS) -c -o display.o display.c

string.o: string.c common.h
	gcc $(CFLAGS) -c -o string.o string.c

prog.o: prog.c common.h
	gcc $(CFLAGS) -c -o prog.o prog.c

start16.o: start16.s
	as --64 -o start16.o start16.s

start64.o: start64.s
	as --64 -o start64.o start64.s

clean:
	rm -f vbr.sys frame.sys *.o parport.out debugger.out bochsout.txt snapshot.txt frame.map

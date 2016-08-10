
cp mt7621_stage_L2.bin uboot_a.bin
echo "0 11"|xxd -r|dd bs=1 count=1 seek=39 of=uboot_a.bin conv=notrunc
chmod 777 uboot_a.bin

count=`stat -c %s u-boot.bin`

bytes=`stat -c %s mt7621_stage_L2.bin`
echo "$(((bytes + 4095)/4096))"

#4k_blocks=$(((bytes + 4095)/4096)*4096-64)
				
dd if=u-boot.bin of=uboot_a.bin bs=1 count=$count seek=$(( ((bytes + 4095)/4096) * 4096 - 64 )) conv=notrunc

/home/kenjo/proj/inteno/mediatek/Uboot/tools/mkimage -A mips -T standalone -C none \
	-a 0xA0200000 -e 0xa0200000 \
	-n "NAND Flash Image" \
	-r DDR3 -s 16 -t 256 -u 32 \
	-y 40 \
	-z 5000 -d uboot_a.bin uboot.img

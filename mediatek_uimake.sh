#!/bin/bash

# mediatek=$1
# if [ ! $1 ]; then
#     echo "Pass the path to mediatek/Uboot as an argument"
#     exit -1
# fi

DDR_CHIP=DEFAULT_DDR3_2048M
CFG_ENV_IS=IN_NAND

cp mt7621_stage_L2.bin uboot_a.bin
./mt7621_ddr.sh uboot_a.bin uboot_a.bin mt7621_ddr_param.txt $DDR_CHIP $CFG_ENV_IS
echo "0 10"|xxd -r|dd bs=1 count=1 seek=38 of=uboot_a.bin conv=notrunc
echo "0 11"|xxd -r|dd bs=1 count=1 seek=39 of=uboot_a.bin conv=notrunc
chmod 777 uboot_a.bin

count=`stat -c %s u-boot.bin`

bytes=`stat -c %s mt7621_stage_L2.bin`
echo "$(((bytes + 4095)/4096))"

#4k_blocks=$(((bytes + 4095)/4096)*4096-64)
				
dd if=u-boot.bin of=uboot_a.bin bs=1 count=$count seek=$(( ((bytes + 4095)/4096) * 4096 - 64 )) conv=notrunc

./tools/mkimage_mediatek -A mips -T standalone -C none \
	-a 0xA0200000 -e 0xa0200000 \
	-n "NAND Flash Image" \
	-r DDR3 -s 16 -t 256 -u 32 \
	-y 40 \
	-z 5000 -d uboot_a.bin uboot.img

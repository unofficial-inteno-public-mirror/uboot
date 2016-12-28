	.set noreorder
	.globl _start
	.section .text

_start:
	nop
1:
	// uncomment this to be able to attach jtag debugger.
//	b	1b
	nop

	/* setup gpio pinmux

	led status green, gpio 8 as gpio function
	
	RALINK_REG(0xbe000060) &= ~(3 << 3); //clear bit 3 & 4
	RALINK_REG(0xbe000060) |= (1 << 3); //set bit 3

	led status red, gpio 11 as gpio function
	led wps (green), gpio 12 as gpio function

	RALINK_REG(0xbe000060) &= ~(3 << 5); //clear bit 5 & 6
	RALINK_REG(0xbe000060) |= (1 << 5); //set bit 5

	*/
	
	li	$11, 0xbe000060
	lw	$12, 0($11)
	and	$12, 0xffffff87	/* (3 << 3) | (3 << 5) */
	or	$12, 0x00000028	/* (1 << 3) | (1 << 5) */	
	sw	$12, 0($11)


	/* set gpio 8,11,12 to output */
	li	$11, 0xbe000600
	lw	$12, 0($11)
	or	$12, 0x00001900	/* 1<<8 | 1<<11 | 1<<12 */
	sw	$12, 0($11)

	/* set status to red, bit 11 -> 1(on) 12&8 -> 1(off)*/
	li	$11, 0xbe000620
	lw	$12, 0($11)
	or	$12, 0x1900	/* 1<<11 1<<8 1<<12 */
	sw	$12, 0($11)
	
/* jump back to 0x89000140 */	
	li	$11, 0x89000140
	j $11
	nop
	

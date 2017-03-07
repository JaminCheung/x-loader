/*
 * Ingenic X1000 TCSM size 12KByte
 */
MEMORY
{
	.tcsm (rx) : ORIGIN = 0xf4001800, LENGTH = 12K
}

OUTPUT_FORMAT("elf32-tradlittlemips", "elf32-tradbigmips", "elf32-tradlittlemips")
OUTPUT_ARCH(mips)
ENTRY(__start)
SECTIONS
{
	.text : { *(.text*) } >.tcsm

	. = ALIGN(4);
	.rodata : { *(SORT_BY_ALIGNMENT(.rodata*)) } >.tcsm

	. = ALIGN(4);
	.data : { *(SORT_BY_ALIGNMENT(.data*)) } >.tcsm

	. = ALIGN(4);
	__image_copy_end = .;

	.bss : {
		. = ALIGN(4);
		__bss_start = .;
		*(.sbss.*)
		*(.bss.*)
		*(COMMON)
		. = ALIGN(4);
		__bss_end = .;
	} >.tcsm

	/DISCARD/ : {
		*(.dynbss)
		*(.dynstr)
		*(.dynamic)
		*(.interp)
		*(.hash)
		*(.gnu.*)
		*(.plt)
		*(.got.plt)
		*(.rel.plt)
		*(.rel.dyn)
	}
}
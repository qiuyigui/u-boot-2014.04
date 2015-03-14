#if 0
/*
 * (C) Copyright 2010-2012
 * Texas Instruments, <www.ti.com>
 *
 * Aneesh V <aneesh@ti.com>
 * Tom Rini <trini@ti.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <config.h>
#include <spl.h>
#include <image.h>
#include <linux/compiler.h>

/* Pointer to as well as the global data structure for SPL */
DECLARE_GLOBAL_DATA_PTR;
gd_t gdata __attribute__ ((section(".data")));

/*
 * In the context of SPL, board_init_f must ensure that any clocks/etc for
 * DDR are enabled, ensure that the stack pointer is valid, clear the BSS
 * and call board_init_f.  We provide this version by default but mark it
 * as __weak to allow for platforms to do this in their own way if needed.
 */
void __weak board_init_f(ulong dummy)
{
	/* Clear the BSS. */
	memset(__bss_start, 0, __bss_end - __bss_start);

	/* Set global data pointer. */
	gd = &gdata;

	board_init_r(NULL, 0);
}

/*
 * This function jumps to an image with argument. Normally an FDT or ATAGS
 * image.
 * arg: Pointer to paramter image in RAM
 */
#ifdef CONFIG_SPL_OS_BOOT
void __noreturn jump_to_image_linux(void *arg)
{
	unsigned long machid = 0xffffffff;
#ifdef CONFIG_MACH_TYPE
	machid = CONFIG_MACH_TYPE;
#endif

	debug("Entering kernel arg pointer: 0x%p\n", arg);
	typedef void (*image_entry_arg_t)(int, int, void *)
		__attribute__ ((noreturn));
	image_entry_arg_t image_entry =
		(image_entry_arg_t) spl_image.entry_point;
	cleanup_before_linux();
	image_entry(0, machid, arg);
}
#endif

#endif 
#include <common.h>
#include <config.h>
#include <spl.h>
#include <image.h>
#include <linux/compiler.h>
typedef u32(*copy_sd_mmc_to_mem)
(u32 channel, u32 start_block, u16 block_size, u32 *trg, u32 init);
/* Pointer to as well as the global data structure for SPL */
DECLARE_GLOBAL_DATA_PTR;
gd_t gdata __attribute__ ((section(".data")));

#define GPA0CON 		( *((volatile unsigned long *)0xE0200000) )		
#define GPA1CON 		( *((volatile unsigned long *)0xE0200020) )

// UARTÏà¹ØŒÄŽæÆ÷
#define ULCON0 			( *((volatile unsigned long *)0xE2900000) )		
#define UCON0 			( *((volatile unsigned long *)0xE2900004) )
#define UFCON0 			( *((volatile unsigned long *)0xE2900008) )
#define UMCON0 			( *((volatile unsigned long *)0xE290000C) )
#define UTRSTAT0 		( *((volatile unsigned long *)0xE2900010) )
#define UERSTAT0 		( *((volatile unsigned long *)0xE2900014) )
#define UFSTAT0 		( *((volatile unsigned long *)0xE2900018) )
#define UMSTAT0 		( *((volatile unsigned long *)0xE290001C) )
#define UTXH0 			( *((volatile unsigned long *)0xE2900020) )
#define URXH0 			( *((volatile unsigned long *)0xE2900024) )
#define UBRDIV0 		( *((volatile unsigned long *)0xE2900028) )
#define UDIVSLOT0 		( *((volatile unsigned long *)0xE290002C) )
#define UINTP 			( *((volatile unsigned long *)0xE2900030) )
#define UINTSP 			( *((volatile unsigned long *)0xE2900034) )
#define UINTM 			( *((volatile unsigned long *)0xE2900038) )

#define UART_UBRDIV_VAL		35
#define UART_UDIVSLOT_VAL	0x1

// Ê±ÖÓÏà¹ØŒÄŽæÆ÷
#define APLL_LOCK 			( *((volatile unsigned long *)0xE0100000) )		
#define MPLL_LOCK 			( *((volatile unsigned long *)0xE0100008) )

#define APLL_CON0 			( *((volatile unsigned long *)0xE0100100) )
#define APLL_CON1 			( *((volatile unsigned long *)0xE0100104) )
#define MPLL_CON 			( *((volatile unsigned long *)0xE0100108) )

#define CLK_SRC0 			( *((volatile unsigned long *)0xE0100200) )
#define CLK_SRC1 			( *((volatile unsigned long *)0xE0100204) )
#define CLK_SRC2 			( *((volatile unsigned long *)0xE0100208) )
#define CLK_SRC3 			( *((volatile unsigned long *)0xE010020c) )
#define CLK_SRC4 			( *((volatile unsigned long *)0xE0100210) )
#define CLK_SRC5 			( *((volatile unsigned long *)0xE0100214) )
#define CLK_SRC6 			( *((volatile unsigned long *)0xE0100218) )
#define CLK_SRC_MASK0 		( *((volatile unsigned long *)0xE0100280) )
#define CLK_SRC_MASK1 		( *((volatile unsigned long *)0xE0100284) )

#define CLK_DIV0 			( *((volatile unsigned long *)0xE0100300) )
#define CLK_DIV1 			( *((volatile unsigned long *)0xE0100304) )
#define CLK_DIV2 			( *((volatile unsigned long *)0xE0100308) )
#define CLK_DIV3 			( *((volatile unsigned long *)0xE010030c) )
#define CLK_DIV4 			( *((volatile unsigned long *)0xE0100310) )
#define CLK_DIV5 			( *((volatile unsigned long *)0xE0100314) )
#define CLK_DIV6 			( *((volatile unsigned long *)0xE0100318) )
#define CLK_DIV7 			( *((volatile unsigned long *)0xE010031c) )

#define CLK_DIV0_MASK		0x7fffffff

#define APLL_MDIV       	0x7d
#define APLL_PDIV       	0x3
#define APLL_SDIV      	 	0x1
#define MPLL_MDIV			0x29b
#define MPLL_PDIV			0xc
#define MPLL_SDIV			0x1

#define set_pll(mdiv, pdiv, sdiv)	(1<<31 | mdiv<<16 | pdiv<<8 | sdiv)
#define APLL_VAL		set_pll(APLL_MDIV,APLL_PDIV,APLL_SDIV)
#define MPLL_VAL		set_pll(MPLL_MDIV,MPLL_PDIV,MPLL_SDIV)


void clock_init()
{
	
	// 1 ÉèÖÃž÷ÖÖÊ±ÖÓ¿ª¹Ø£¬ÔÝÊ±²»Ê¹ÓÃPLL
	CLK_SRC0 = 0x0;

	
	// 2 ÉèÖÃËø¶šÊ±Œä£¬Ê¹ÓÃÄ¬ÈÏÖµŒŽ¿É
	// ÉèÖÃPLLºó£¬Ê±ÖÓŽÓFinÌáÉýµœÄ¿±êÆµÂÊÊ±£¬ÐèÒªÒ»¶šµÄÊ±Œä£¬ŒŽËø¶šÊ±Œä 			
	APLL_LOCK = 0x0000FFFF;          			
	MPLL_LOCK = 0x0000FFFF;					

	
	// 3 ÉèÖÃ·ÖÆµ
	CLK_DIV0 = 0x14131440;

	// 4 ÉèÖÃPLL
	// FOUT= MDIV * FIN / (PDIV*2^(SDIV-1)) = 0x7d*24/(0x3*2^(1-1))=1000 MHz
	APLL_CON0 = APLL_VAL;
	// FOUT = MDIV*FIN/(PDIV*2^SDIV)=0x29b*24/(0xc*2^1)= 667 MHz
	MPLL_CON  = MPLL_VAL;					

	// 5 ÉèÖÃž÷ÖÖÊ±ÖÓ¿ª¹Ø£¬Ê¹ÓÃPLL
	CLK_SRC0 = 0x10001111;
}
 
void uart_init (void)  
{  
    // 1 ÅäÖÃÒýœÅÓÃÓÚRX/TX¹ŠÄÜ
	GPA0CON = 0x22222222;
	GPA1CON = 0x2222;

	// 2 ÉèÖÃÊýŸÝžñÊœµÈ
	// Ê¹ÄÜfifo
	UFCON0 = 0x0;
	// ÎÞÁ÷¿Ø
	UMCON0 = 0x0;
	// ÊýŸÝÎ»:8, ÎÞÐ£Ñé, Í£Ö¹Î»: 1
	ULCON0 = 0x3;
	// Ê±ÖÓ£ºPCLK£¬œûÖ¹ÖÐ¶Ï£¬Ê¹ÄÜUART·¢ËÍ¡¢œÓÊÕ
	UCON0  = 0x5;  				

	// 3 ÉèÖÃ²šÌØÂÊ
	UBRDIV0 = UART_UBRDIV_VAL;
	UDIVSLOT0 = UART_UDIVSLOT_VAL;
}  
void uart_putc (char c)
{
	

// Èç¹ûTX FIFOÂú£¬µÈŽý
	while (!(UTRSTAT0 & (1<<2)));
	// ÐŽÊýŸÝ
	UTXH0 = c;   
}

char uart_getc (void)
{
	
// Èç¹ûRX FIFO¿Õ£¬µÈŽý
	while (!(UTRSTAT0 & (1<<0)));
	// È¡ÊýŸÝ
	return URXH0;
} 
void uart_puts (char *s)
{
	while (*s != '\0') {
		uart_putc(*s++);
	}
} 
///////////////



 int itos (int tmp, char *buf, int where)
{
	int count = 0;
	int i;
	char buf2[1024];

	do {
		buf2[count] = tmp % 10 + 48;
		count++;
		tmp = tmp / 10;
	} while (tmp != 0);

	for (i=0; i<count; i++) {
		buf[where] = buf2[count-i-1];
		where++;
	}

	return where;
}

 int itohex (int tmp, char *buf, int where)
{
	//int count = 0;
	int i;
	//char buf2[1024];
	char * hex = "0123456789ABCDEF";

	for (i=0; i<8; i++) {
		buf[where++] = hex[(tmp>>(32-(i+1)*4))&0xf]; 
	}

	return where;
}

 int vsprintf (char *buf, const char *fmt, va_list args)
{
	int i=0;
	int tmp;
	char *tmp2;

//	va_start(args, fmt);

	for (; *fmt ; ++fmt) {
		if (*fmt != '%') {
			//ADDCH(buf, *fmt);
			buf[i] = *fmt;
			i++;
			//printf ("%c\n", *fmt);
			continue;
		}

		++fmt;
		switch (*fmt) {
			case 'd':
				tmp = va_arg(args, int);
				i = itos (tmp, buf, i);
				break;
			case 'c':
				tmp = va_arg(args, int);
				buf[i] = (char)tmp;
				i++;
				break;
			case 'x':
				tmp = va_arg(args, int);
				i = itohex(tmp, buf, i);
				break;
			case 's':
				tmp2 = va_arg(args, char *);
				while (*tmp2 != '\0') {
					buf[i] = *tmp2;
					i++;
					tmp2++;
				}
				break;
		}
	}

//	va_end(args);

	buf[i] = '\0';

	return i;
}


int printf(const char *fmt, ...)
{
	va_list args;
	int i;
	char printbuffer[1024];

	va_start(args, fmt);

	/* For this to work, printbuffer must be larger than
	 * anything we ever want to print.
	 */
	//i = vscnprintf(printbuffer, sizeof(printbuffer), fmt, args);
	i = vsprintf (printbuffer, fmt, args);
	va_end(args);

	/* Print the string */
	//puts(printbuffer);
	uart_puts(printbuffer);
	
	return i;
}
//////////////
void copy_uboot_to_ram(void)
{
	ulong ch;
	ch = *(volatile u32 *)(0xD0037488);
	copy_sd_mmc_to_mem copy_bl2 =
	(copy_sd_mmc_to_mem) (*(u32 *) (0xD0037F98));
	u32 ret;
	printf ("boot mmc chanel: %x\r\n", ch);
	if (ch == 0xEB000000) {
	ret = copy_bl2(0, 49, 1024, CONFIG_SYS_TEXT_BASE, 0);
	}
	if (ret == 0) {
	printf ("copy error\r\n");
	while (1);
	}
	else
	return;
}
/*
 * In the context of SPL, board_init_f must ensure that any clocks/etc for
 * DDR are enabled, ensure that the stack pointer is valid, clear the BSS
 * and call board_init_f.  We provide this version by default but mark it
 * as __weak to allow for platforms to do this in their own way if needed.
 */

void __weak board_init_f(ulong dummy)
{
 
__attribute__((noreturn)) void (*uboot)(void);
  	clock_init();
	uart_init();
	copy_uboot_to_ram();
	printf ("jump to u-boot image\r\n");
	/* Jump to U-Boot image */
	uboot = (void *)CONFIG_SYS_TEXT_BASE;

	(*uboot)();
}

/*
 * This function jumps to an image with argument. Normally an FDT or ATAGS
 * image.
 * arg: Pointer to paramter image in RAM
 */
#ifdef CONFIG_SPL_OS_BOOT
void __noreturn jump_to_image_linux(void *arg)
{
	debug("Entering kernel arg pointer: 0x%p\n", arg);
	typedef void (*image_entry_arg_t)(int, int, void *)
		__attribute__ ((noreturn));
	image_entry_arg_t image_entry =
		(image_entry_arg_t) spl_image.entry_point;
	cleanup_before_linux();
	image_entry(0, CONFIG_MACH_TYPE, arg);
}
#endif


#ifndef IO_HANDLER_H_
#define IO_HANDLER_H_
#include "alt_types.h"
#include "system.h"

// These addresses are defined in system.h generated by Eclipse, and they
// should have been matched to the addresses shown in Qsys.
// If for any reason they are not correctly defined, you can replace these
// with the addresses you read in Qsys.
#define otg_hpi_address			(volatile int*) 	0x00000080
#define otg_hpi_data			(volatile int*)	    0x00000070
#define otg_hpi_r				(volatile char*)	0x00000060
#define otg_hpi_cs				(volatile char*)	0x00000040//FOR SOME REASON CS BASE BEHAVES WEIRDLY MIGHT HAVE TO SET MANUALLY
#define otg_hpi_w				(volatile char*)	0x00000050
#define otg_hpi_reset			(volatile char*)	0x00000030
#define keycode_base				(volatile char*)	0x00000090


void IO_write(alt_u8 Address, alt_u16 Data);
alt_u16 IO_read(alt_u8 Address);
void IO_init(void);


#endif

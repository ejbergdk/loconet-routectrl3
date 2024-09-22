/*
 * flashmem.h
 *
 * Tools to handle data in flash potentially crossing a 64KB boundary.
 *
 * Created: 22-09-2024 12:25:42
 *  Author: Mikael Ejberg Pedersen
 */

#ifndef FLASHMEM_H_
#define FLASHMEM_H_

#ifdef __AVR_HAVE_ELPMX__
// AVR supports ELPM with Z+ so tables may be located above 64KB. Use 24-bit pointer
#define FLASHMEM __memx
#else
// AVR doesn't support data in flash above 64KB. Use normal 16-bit pointer
#define FLASHMEM __flash
#endif


#endif /* FLASHMEM_H_ */

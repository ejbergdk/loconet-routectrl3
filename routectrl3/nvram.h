/*
 * nvram.h
 *
 * Helper functions for interfacing to non-volatile RAM.
 * Compatible with
 *   EERAM: 47C04, 47C16, 47L04, 47L16.
 *   FRAM:  MB85RC04, MB85RC16, MB85RC64, MB85RC128, MB85RC256.
 *
 * Created: 13-12-2025 13:50:06
 *  Author: Mikael Ejberg Pedersen
 */


#ifndef NVRAM_H_
#define NVRAM_H_

#include <stdbool.h>
#include <stdint.h>


/**
 * Init NVRAM module.
 *
 * Call once at startup.
 */
extern void     nvram_init(void);

/**
 * Update NVRAM module.
 *
 * Call regularly from mainloop.
 */
extern void     nvram_update(void);

/**
 * NVRAM ready.
 *
 * @return True if NVRAM module is initialized and ready.
 */
extern bool     nvram_ready(void);

/**
 * Read from NVRAM.
 *
 * @param adr   Addres to read from
 * @param buf   Pointer to buffer to put read data in
 * @param len   Number of bytes to read
 *
 * @return True if read operation is started
 */
extern bool     nvram_read(uint16_t adr, uint8_t *buf, uint16_t len);

/**
 * Write single byte to NVRAM.
 *
 * Writes are buffered, so no need to wait for NVRAM to be ready.
 *
 * @param adr   Address to write to
 * @param data  Byte to write
 */
extern void     nvram_write(uint16_t adr, uint8_t data);

#endif /* NVRAM_H_ */

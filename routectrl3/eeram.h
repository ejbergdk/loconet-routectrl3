/*
 * eeram.h
 *
 * Helper functions for interfacing to EERAM.
 * Compatible with: 47C04, 47C16, 47L04, 47L16.
 *
 * Created: 13-12-2025 13:50:06
 *  Author: Mikael Ejberg Pedersen
 */


#ifndef EERAM_H_
#define EERAM_H_

#include <stdbool.h>
#include <stdint.h>


/**
 * Init EERAM module.
 *
 * Call once at startup.
 */
extern void     eeram_init(void);

/**
 * Update EERAM module.
 *
 * Call regularly from mainloop.
 */
extern void     eeram_update(void);

/**
 * EERAM ready.
 *
 * @return True if EERAM module is initialized and ready.
 */
extern bool     eeram_ready(void);

/**
 * Read from EERAM.
 *
 * @param adr   Addres to read from
 * @param buf   Pointer to buffer to put read data in
 * @param len   Number of bytes to read
 *
 * @return True if read operation is started
 */
extern bool     eeram_read(uint16_t adr, uint8_t *buf, uint16_t len);

/**
 * Write single byte to EERAM.
 *
 * Writes are buffered, so no need to wait for EERAM to be ready.
 *
 * @param adr   Address to write to
 * @param data  Byte to write
 */
extern void     eeram_write(uint16_t adr, uint8_t data);

#endif /* EERAM_H_ */

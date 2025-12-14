/*
 * twim.h
 *
 * TWI master driver for AVR DA.
 * Interrupt driven.
 * Not tested in a multi-master setup.
 *
 * Created: 21-09-2025 12:58:08
 *  Author: Mikael Ejberg Pedersen
 */


#ifndef TWIM_H_
#define TWIM_H_

#include <stdbool.h>
#include <stdint.h>


/**
 * TWIM status
 */
typedef enum
{
    TWIM_STATUS_DONE,
    TWIM_STATUS_NODEVICE,
    TWIM_STATUS_ERROR
} twim_status_t;

/**
 * TWIM callback function prototype.
 *
 * NOTE: Do NOT start a new TWI read or write from within this callback.
 */
typedef void    (twim_cb) (twim_status_t);


/**
 * Init TWI master module.
 *
 * Call once at startup.
 */
extern void     twim_init(void);

/**
 * Update TWI master module.
 *
 * Call regularly from mainloop.
 */
extern void     twim_update(void);

/**
 * TWIM ready or busy.
 *
 * @return      True if ready for a new transaction.
 */
extern bool     twim_ready(void);

/**
 * Write data to TWI
 *
 * @param adr   Address (8-bit, already left-shifted).
 * @param buf   Ptr to data to write. Must be available until callback is called.
 * @param len   Length of data to write.
 * @param cb    Function callback with status when transaction is done.
 *
 * @return      True if started successfully (wait for callback), false if not (no callback).
 */
extern bool     twim_write(uint8_t adr, const uint8_t *buf, uint16_t len, twim_cb *cb);

/**
 * Read data from TWI
 *
 * @param adr   Address (8-bit, already left-shifted).
 * @param buf   Ptr for received data.
 * @param len   Length of data to receive.
 * @param cb    Function callback with status when transaction is done.
 *
 * @return      True if started successfully (wait for callback), false if not (no callback).
 */
extern bool     twim_read(uint8_t adr, uint8_t *buf, uint16_t len, twim_cb *cb);

/**
 * Write and read data on TWI (using repeated start)
 *
 * @param adr   Address (8-bit, already left-shifted).
 * @param wbuf  Ptr to data to write. Must be available until callback is called.
 * @param wlen  Length of data to write.
 * @param rbuf  Ptr for received data.
 * @param rlen  Length of data to receive.
 * @param cb    Function callback with status when transaction is done.
 *
 * @return      True if started successfully (wait for callback), false if not (no callback).
 */
extern bool     twim_write_read(uint8_t adr, const uint8_t *wbuf, uint16_t wlen, uint8_t *rbuf, uint16_t rlen,
                                twim_cb *cb);

#endif /* TWIM_H_ */

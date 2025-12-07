/*
 * twim.h
 *
 * Created: 21-09-2025 12:58:08
 *  Author: Mikael Ejberg Pedersen
 */


#ifndef TWIM_H_
#define TWIM_H_

#include <stdbool.h>
#include <stdint.h>


/*
 * TWIM status
 */
typedef enum
{
    TWIM_STATUS_DONE,
    TWIM_STATUS_NODEVICE,
    TWIM_STATUS_ERROR
} twim_status_t;

/*
 * TWIM callback function prototype.
 */
typedef void    (twim_cb) (twim_status_t);


/*
 * Init TWI master module.
 *
 * Call once at startup.
 */
extern void     twim_init(void);

/*
 * Update TWI master module.
 *
 * Call regularly from mainloop.
 */
extern void     twim_update(void);

/*
 * TWIM ready or busy.
 *
 * @return      True if ready for a new transaction.
 */
extern bool     twim_ready(void);

/*
 * Send data to TWI
 *
 * @param adr   Address (8-bit, already left-shifted).
 * @param buf   Ptr to data to send. Must be available until callback is called.
 * @param len   Length of data to send.
 * @param cb    Function callback with status when transaction is done.
 *
 * @return      True if started successfully (wait for callback), false if not (no callback).
 */
extern bool     twim_send(uint8_t adr, const uint8_t *buf, uint8_t len, twim_cb *cb);

/*
 * Receive data from TWI
 *
 * @param adr   Address (8-bit, already left-shifted).
 * @param buf   Ptr for received data.
 * @param len   Length of data to receive.
 * @param cb    Function callback with status when transaction is done.
 *
 * @return      True if started successfully (wait for callback), false if not (no callback).
 */
extern bool     twim_recv(uint8_t adr, uint8_t *buf, uint8_t len, twim_cb *cb);

/*
 * Send and receive data on TWI
 *
 * @param adr   Address (8-bit, already left-shifted).
 * @param txbuf Ptr to data to send. Must be available until callback is called.
 * @param txlen Length of data to send.
 * @param rxbuf Ptr for received data.
 * @param rxlen Length of data to receive.
 * @param cb    Function callback with status when transaction is done.
 *
 * @return      True if started successfully (wait for callback), false if not (no callback).
 */
extern bool     twim_send_recv(uint8_t adr, const uint8_t *txbuf, uint8_t txlen, uint8_t *rxbuf, uint8_t rxlen,
                               twim_cb *cb);

#endif /* TWIM_H_ */

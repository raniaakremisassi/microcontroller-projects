#ifndef PROTOCOL_H
#define PROTOCOL_H

#define PROTOCOL_COMM_TX_PILOT	_BV(0)
#define PROTOCOL_COMM_RX_PILOT	_BV(1)
#define PROTOCOL_COMM_TX_PC		_BV(2)
#define PROTOCOL_COMM_RX_PC		_BV(3)

#include <math.h>
#include "../../main.h"

void protocol_poll();

/*
 * Sends a control message to the pilot.
 */
void protocol_send_control(control_t control);
void protocol_send_kill();

/*
 * Sends a reset attitude message to the pilot.
 */
void protocol_send_reset_attitude();

/*
 * Sends a reset calibrate message to the pilot.
 */
void protocol_send_calibrate();

void protocol_send_toggle_telemetry();

void protocol_send_diag(char *);

/*
 * Clears the rx / tx activity state; call this from main after updating status LCD.
 */
void protocol_clear_comm_state();

/*
 * Gets whether a given bit is set in activity state.
 */
uint8_t protocol_comm_state(uint8_t bit);

#endif

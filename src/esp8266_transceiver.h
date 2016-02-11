/*
 * \file esp8266_transceiver.h
 * \brief The file specifies the main functions of the bottom communication
 * layer
 * \details The module uses the UART in order to communicate with the ESP8266.
 * It demultiplexes the read data stream and decodes basic data structures. In
 * order to reduce the time spent in the interrupt routines, the module deploys
 * a round robin buffer. The actual decoding is out-sourced in a separate task.
 * The task function esp8266_transc_tick() has to be called frequently in order
 * to process received messages. On decoding a message or on receiving a status
 * code a call-back function is executed.
 * 
 * \author Michael Spiegel, <michael.h.spiegel@gmail.com>
 *
 * Copyright (C) 2015 Michael Spiegel
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef ESP8266_TRANSCEIVER_H_
#define ESP8266_TRANSCEIVER_H_

#include "error.h"

#include <stdint.h>

/**
 * \brief Defines a callback pointer which indicates a received status message
 * \param status The received and decoded status
 */
typedef void (*esp8266_transc_statusReceived)(status_t status);

/**
 * \brief Callback pointer which points to a message processing function.
 * \details The function is executed if a message was received.
 * \param status The status of the message
 * \param channel The message's channel number. The value ranges from zero to
 * three.
 * \param size The number of data bytes which were received
 * \param rrbID An index to the message's first byte in the round robin buffer.
 * It is advised to use protected access mechanisms only. The message may not
 * be organized in a consecutive memory block. I.e. the memory may eventually
 * wrap.
 */
typedef void (*esp8266_transc_messageReceived)(status_t status, uint8_t channel,
		uint8_t size, uint8_t rrbID);

/**
 * \brief Initializes the module
 * \details The function has to be called before any other function is used. It
 * is assumed that both function pointer point to valid functions. Additionally,
 * interrupts need to be globally disabled.
 * \param statusCB A callback function which is executed on receiving a valid
 * status message. The function is executed outside an interrupt context.
 * \param messageCB A callback function which is executed on receiving a new
 * message. The function is always executed outside an interrupt context. Passed
 * memory regions are only valid until the function returns.
 */
void esp8266_transc_init(esp8266_transc_statusReceived statusCB,
		esp8266_transc_messageReceived messageCB);

/**
 * \brief Decodes any received message
 * \details The function has to be executed frequently in order to process any
 * received data. If no data was received so far the function will immediately
 * return. Otherwise some registered callback functions may be called.
 */
void esp8266_transc_tick(void);

/**
 * \brief Send the given packet
 * \details The packet will be transmit without appending any additional
 * characters. The buffer must be valid until the response code is sent and the
 * status callback is executed. It is not allowed to transmit two packets
 * simultaneously.
 * \param buffer The memory region of the packet to transmit. It has to contain
 * at least size bytes.
 * \param size The number of bytes to transmit. It has to be greater or equal
 * than one.
 */
void esp8266_transc_send(uint8_t *buffer, uint8_t size);


#endif /* ESP8266_TRANSCEIVER_H_ */
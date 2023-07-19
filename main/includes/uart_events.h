/*
 * uart_events.h
 *
 *  Created on: 2023年6月29日
 *      Author: owen
 */

#ifndef MAIN_INCLUDES_UART_EVENTS_H_
#define MAIN_INCLUDES_UART_EVENTS_H_


void uart_event_task(void *pvParameters);
void uart_init(void);
void rx_task(void *arg);
void tx_task(void *arg);

#endif /* MAIN_INCLUDES_UART_EVENTS_H_ */

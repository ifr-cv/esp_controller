/*
 * status.h
 *
 *  Created on: 2023年7月4日
 *      Author: owen
 */

#ifndef MAIN_INCLUDES_STATUS_H_
#define MAIN_INCLUDES_STATUS_H_

#include "esp_system.h"

#pragma pack(1)
struct CommunicationStatus{
   uint16_t communication_delay : 9; //0~400
   uint8_t packet_loss_rate : 7;    //0~100
};
#pragma pack()

extern struct CommunicationStatus CS;

#pragma pack(1)
struct SocketMsg {
    uint8_t head : 8;
    uint8_t btn1 : 2;
    uint8_t btn2 : 1;
    uint8_t btn3 : 1;
    uint8_t btn4 : 1;
    uint8_t pole : 3;
    uint16_t ch1 : 11;
    uint16_t ch2 : 11;
    uint8_t reverse1 : 2;
    uint16_t ch3 : 11;
    uint16_t ch4 : 11;
    uint8_t reverse2 : 2;
    int8_t sc : 8;
    uint16_t sequence : 16;
};
#pragma pack()

extern struct SocketMsg smsg;

extern uint32_t rev_count;
extern int32_t missing;


#endif /* MAIN_INCLUDES_STATUS_H_ */

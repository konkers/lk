#ifndef __PLATFORM_STM32_CAN_H
#define __PLATFORM_STM32_CAN_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    unsigned id:11;  // Standard CAN identifier.
    unsigned id_ex:18;  // Extended CAN identifier.
    unsigned rtr:1;  // Remote transmit request.
    unsigned ide:1;  // Identifier extension.
    unsigned pad:1;

    uint8_t dlc;
    uint8_t data[8];
} __attribute__((packed)) can_msg_t;

void can_init(void);
void can_send(const can_msg_t *msg);
bool can_recv(can_msg_t *msg, bool wait);

#endif  // __PLATFORM_STM32_CAN_H

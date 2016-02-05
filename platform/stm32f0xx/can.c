#include <platform/can.h>

#include <stdbool.h>

#include <arch/arm/cm.h>
#include <stm32f0xx_can.h>
#include <stm32f0xx_rcc.h>

void stm32_CAN_IRQ(void)
{
    arm_cm_irq_entry();
    bool resched = false;


    arm_cm_irq_exit(resched);
}

void can_init(void) {
    // Enable CAN peripheral clock.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN, ENABLE);

    // Reset CAN registers.
    CAN_DeInit(CAN);

    CAN_InitTypeDef init;

    // Config for loopback.
    init.CAN_TTCM = DISABLE;
    init.CAN_ABOM = DISABLE;
    init.CAN_AWUM = DISABLE;
    init.CAN_NART = DISABLE;
    init.CAN_RFLM = DISABLE;
    init.CAN_TXFP = DISABLE;
    init.CAN_Mode = CAN_Mode_LoopBack;
    init.CAN_SJW = CAN_SJW_1tq;

    // CAN Baudrate = 125kbps (CAN clocked at 36 MHz)
    // XXX: this is probably wrong running at 48MHz
    init.CAN_BS1 = CAN_BS1_9tq;
    init.CAN_BS2 = CAN_BS2_8tq;
    init.CAN_Prescaler = 16;

    CAN_Init(CAN, &init);

    CAN_FilterInitTypeDef filter;
    filter.CAN_FilterNumber = 0;
    filter.CAN_FilterMode = CAN_FilterMode_IdMask;
    filter.CAN_FilterScale = CAN_FilterScale_32bit;
    filter.CAN_FilterIdHigh = 0x0000;
    filter.CAN_FilterIdLow = 0x0000;
    filter.CAN_FilterMaskIdHigh = 0x0000;
    filter.CAN_FilterMaskIdLow = 0x0000;
    filter.CAN_FilterFIFOAssignment = 0;
    filter.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&filter);
}

void can_send(const can_msg_t *msg) {
    CanTxMsg CAN_msg;
    uint8_t mailbox;
    int i;

    CAN_msg.StdId = msg->id;
    CAN_msg.ExtId = msg->id_ex;
    CAN_msg.IDE = msg->ide;
    CAN_msg.RTR = msg->rtr;
    CAN_msg.DLC = msg->dlc;

    for (i = 0; i < msg->dlc; i++) {
        CAN_msg.Data[i] = msg->data[i];
    }

    mailbox = CAN_Transmit(CAN, &CAN_msg);
    while(CAN_TransmitStatus(CAN, mailbox) != CANTXOK) {}
}

bool can_recv(can_msg_t *msg, bool wait) {
    CanRxMsg CAN_msg;
    int i;

    while(CAN_MessagePending(CAN, CAN_FIFO0) < 1) {}

    CAN_Receive(CAN, CAN_FIFO0, &CAN_msg);
    msg->id = CAN_msg.StdId;
    msg->id_ex = CAN_msg.ExtId;
    msg->ide = CAN_msg.IDE;
    msg->rtr = CAN_msg.RTR;
    msg->dlc = CAN_msg.DLC;

    for (i = 0; i < msg->dlc; i++) {
        msg->data[i] = CAN_msg.Data[i];
    }

    return true;
}

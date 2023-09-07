#include "mbed.h"
#include "CAN.h"

#include "constants.h"

CAN can1(CAN1_RD, CAN1_TD); // CAN (PinName rd, PinName td);
// CAN can2(CAN2_RD, CAN2_TD); // CAN (PinName rd, PinName td);

char counter = 0;

Thread canReadThread;

EventQueue queueRead(32 * EVENTS_EVENT_SIZE);

unsigned int filterMask = 0x00000001;
unsigned int filter1Id = 0x00000001;
// unsigned int filter2Id = 0x00000000;

void readCAN1() {
  printf("reading CAN1...\n");
  CANMessage msg;
  if (can1.read(msg)) {
    printf("CAN1 Message received: %d\n", msg.data[0]);
  }
}

// void readCAN2() {
//   printf("reading CAN2...\n");
//   CANMessage msg;
//   if (can2.read(msg)) {
//     printf("CAN2 Message received: %d\n", msg.data[0]);
//   }
// }

void setUpFilter() {
  can1.filter(filter1Id, filterMask, CANStandard, 1);
  //   can2.filter(filter2Id, filterMask, CANStandard, 14);
}

void can1ISR() {
  queueRead.call(readCAN1);
}

// void can2ISR() {
//   printf("ISR CAN2\n");
//   queue.call(readCAN2);
// }

int main() {
  printf("init start\n");
  can1.frequency(1000E3);
  can1.mode(CAN::Normal);
  //   can2.frequency(1000000);
  //   can2.mode(CAN::Normal);

  setUpFilter();
  printf("can filter fin\n");

  can1.attach(can1ISR, CAN::RxIrq);
  //   can2.attach(can2ISR, CAN::RxIrq);
  canReadThread.start(callback(&queueRead, &EventQueue::dispatch_forever));
  printf("can attach fin\n");
}
#include "mbed.h"
#include "CAN.h"

#include "constants.h"

CAN can1(CAN1_RD, CAN1_TD); // CAN (PinName rd, PinName td);
// CAN can2(CAN2_RD, CAN2_TD); // CAN (PinName rd, PinName td);

char counter = 0;

Ticker ticker;
Thread canReadThread;
Thread canWriteThread;

EventQueue queueRead(32 * EVENTS_EVENT_SIZE);
EventQueue queueWrite(32 * EVENTS_EVENT_SIZE);

unsigned int filterMask = 0x00000001;
unsigned int filter1Id = 0x00000001;
// unsigned int filter2Id = 0x00000000;

int sendAll() {
  int returnVal = 1;
  for (int i = 1; i <= 8; i++) {
    int result = can1.write(CANMessage(i, &counter, 1));
    printf("%p Message sent: %d\n", &can1, counter++);
    if (result != 1) {
      printf("Failed to send data.\n");
    }
    returnVal &= result;
  }

  return returnVal;
}

void sendCANTask() {
  sendAll();
  //   sendAll(&can2);
}

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

void can1Write() {
  queueWrite.call(sendAll);
}

// void can2ISR() {
//   printf("ISR CAN2\n");
//   queue.call(readCAN2);
// }

int main() {
  printf("init start\n");
  can1.frequency(5 * 100 * 1000);
  can1.mode(CAN::Normal);
  //   can2.frequency(1000000);
  //   can2.mode(CAN::Normal);

  setUpFilter();
  printf("can filter fin\n");

  can1.attach(can1ISR, CAN::RxIrq);
  //   can2.attach(can2ISR, CAN::RxIrq);
  canReadThread.start(callback(&queueRead, &EventQueue::dispatch_forever));
  printf("can attach fin\n");

  canWriteThread.start(callback(&queueWrite, &EventQueue::dispatch_forever));
  ticker.attach(can1Write, 1s);
}
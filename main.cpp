#include "mbed.h"

#include "CAN.h"
#include "NoMutexCAN.h"

#include "constants.h"

NoMutexCAN can1(CAN1_RD, CAN1_TD); // CAN (PinName rd, PinName td);
NoMutexCAN can2(CAN2_RD, CAN2_TD); // CAN (PinName rd, PinName td);

char counter = 0;

Ticker ticker;

unsigned int filterMask = 0x00000001;
unsigned int filter1Id = 0x00000001;
unsigned int filter2Id = 0x00000000;

int sendAll(CAN *can) {
  int returnVal = 1;
  for (int i = 0; i < 8; i++) {
    int result = (*can).write(CANMessage(i, &counter, 1));
    printf("Message sent: %d\n", counter);
    returnVal &= result;
  }

  return returnVal;
}

void sendCANTask() {
  sendAll(&can1);
  ThisThread::sleep_for(10ms);

  sendAll(&can2);
  ThisThread::sleep_for(10ms);
}

void readCAN1() {
  CANMessage msg;
  if (can1.read(msg)) {
    printf("Message received: %d\n", msg.data[0]);
  }
}

void readCAN2() {
  CANMessage msg;
  if (can2.read(msg)) {
    printf("Message received: %d\n", msg.data[0]);
  }
}

void setUpFilter() {
  can1.filter(filter1Id, filterMask, CANAny, 1);
  can2.filter(filter2Id, filterMask, CANAny, 14);
}

int main() {
  printf("init start\n");
  can1.frequency(1000000);
  can2.frequency(1000000);
  printf("can freq fin\n");

  setUpFilter();
  printf("can filter fin\n");

  ticker.attach(&sendCANTask, 5000ms);
  printf("ticker attach fin\n");
  printf("init fin\n");

  while (1) {
    readCAN1();
    ThisThread::sleep_for(10ms);
    readCAN2();
    ThisThread::sleep_for(10ms);
  }
}
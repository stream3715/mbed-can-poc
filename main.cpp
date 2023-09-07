#include "mbed.h"

CAN can1(PB_8, PB_9);
EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread t;

void readCAN() {
  printf("readCAN triggered\n");

  CANMessage msg;
  if (can1.read(msg)) {
    printf("Msg ID: %d, data: %d\n", msg.id, msg.data[0]);
  }
}

void readISR() { queue.call(readCAN); }

int main() {
  printf("F446\n");
  can1.frequency(1000E3);
  can1.attach(callback(readISR));
  t.start(callback(&queue, &EventQueue::dispatch_forever));
}
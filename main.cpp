#include "mbed.h"

CAN can1(PB_8, PB_9);
EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread t;

int main() {
  printf("F446\n");
  can1.frequency(500E3);
  t.start(callback(&queue, &EventQueue::dispatch_forever));

  CANMessage msg;
  while (1) {
    if (can1.read(msg)) {
      printf("Msg ID: %d, data: %d\n", msg.id, msg.data[0]);
    }
    thread_sleep_for(100);
  }
}
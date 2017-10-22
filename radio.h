#ifndef _RADIO_H
#define _RADIO_H

#include <stdbool.h>
#include <stdint.h>

#define RADIO_MAX_PACKET_SIZE 128

void radio_init();
bool radio_is_data_available();
int radio_get_packet(uint8_t packet[]);
void radio_send_packet(uint8_t packet[], int length);

#endif

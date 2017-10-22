#ifndef _EM2420_H
#define _EM2420_H

#define EM2420_STROBE_SNOP 0x00
#define EM2420_STROBE_SXOSCON 0x01

void em2420_init();
uint8_t em2420_send_command_strobe(uint8_t address);

#endif

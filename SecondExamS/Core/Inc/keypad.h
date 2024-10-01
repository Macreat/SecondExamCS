#ifndef INC_KEYPAD_H_
#define INC_KEYPAD_H_

#include <stdint.h>
// Function to scan for a key press based on the GPIO pin

uint8_t keypad_scan(uint16_t GPIO_Pin);

#endif /* INC_KEYPAD_H_ */



#include "keypad.h"

#include "main.h"


static uint8_t keypad_debounce(uint16_t GPIO_Pin)
{

	/*
	 *
	 * Debouncing (keypad_debounce):
Ensures that key presses are not registered multiple times due to electrical noise.
Uses a delay of 200ms to filter out multiple key presses in a short time span.
	 *
	 *
	 */
	//just managemente for DEBOUNCE delay
	static uint16_t last_pressed = 0xFFFF;
	static uint32_t last_tick = 0;

	if (last_pressed == GPIO_Pin) {
		if (HAL_GetTick() < (last_tick + 200)) {
			return 0;
		}
	}
	last_pressed = GPIO_Pin;
	last_tick = HAL_GetTick();

	return 1;
}




/*
 *
 * Row Scanning (keypad_scan_row):
Activates each row sequentially by setting the corresponding GPIO pin low (RESET), and then reads the column pins to see if any key is pressed.
If a key press is detected, the function returns the row number where the key was pressed.
 *
 *
 */
uint8_t keypad_scan_row(GPIO_TypeDef *COLx_GPIO_Port, uint16_t COLx_Pin)
{
	uint8_t row_pressed = 0xFF;
	HAL_GPIO_WritePin(ROW_1_GPIO_Port, ROW_1_Pin, GPIO_PIN_RESET);
	if (HAL_GPIO_ReadPin(COLx_GPIO_Port, COLx_Pin) == 0) {
		row_pressed =  1;
		goto row_scan_end;
	}
	HAL_GPIO_WritePin(ROW_2_GPIO_Port, ROW_2_Pin, GPIO_PIN_RESET);
	if (HAL_GPIO_ReadPin(COLx_GPIO_Port, COLx_Pin) == 0) {
		row_pressed =  2;
		goto row_scan_end;
	}
	HAL_GPIO_WritePin(ROW_3_GPIO_Port, ROW_3_Pin, GPIO_PIN_RESET);
	if (HAL_GPIO_ReadPin(COLx_GPIO_Port, COLx_Pin) == 0) {
		row_pressed =  3;
		goto row_scan_end;
	}
	HAL_GPIO_WritePin(ROW_4_GPIO_Port, ROW_4_Pin, GPIO_PIN_RESET);
	if (HAL_GPIO_ReadPin(COLx_GPIO_Port, COLx_Pin) == 0) {
		row_pressed =  4;
		goto row_scan_end;
	}
row_scan_end:
	HAL_GPIO_WritePin(ROW_1_GPIO_Port, ROW_1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ROW_2_GPIO_Port, ROW_2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ROW_3_GPIO_Port, ROW_3_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ROW_4_GPIO_Port, ROW_4_Pin, GPIO_PIN_SET);
	return row_pressed; // not detected
}

uint8_t keypad_chars[4][4] = {
		{'1', '2', '3', 'A'},
		{'4', '5', '6', 'B'},
		{'7', '8', '9', 'C'},
		{'*', '0', '#', 'D'},
};
/*
 * Purpose:

The main function that processes the keypress by calling keypad_debounce() to ensure the press is valid, then calls keypad_scan_row() to detect which row is active, and finally returns the corresponding key from the keypad_chars array.
Parameters:

GPIO_Pin: The GPIO pin where the keypress is detected (one of the column pins).
Return Value:

Returns the ASCII character of the key that was pressed, e.g., '1', 'A', '#', or '*'. If no valid key is detected, it returns 0xFF.
 *
 *
 *
 *
 */
uint8_t keypad_scan(uint16_t GPIO_Pin)
{
	uint8_t key_pressed = 0xFF;

	if (keypad_debounce(GPIO_Pin) == 0) { // to ensure the press is valid,
		return key_pressed;
	}
	uint8_t row = 0xFF;
	switch (GPIO_Pin) {
	case COL_1_Pin:
		row = keypad_scan_row(COL_1_GPIO_Port, COL_1_Pin);
		key_pressed = keypad_chars[row - 1][1 - 1];
		break;

	case COL_2_Pin:
		row = keypad_scan_row(COL_2_GPIO_Port, COL_2_Pin);
		key_pressed = keypad_chars[row - 1][2 - 1];
		break;

	case COL_3_Pin:
			row = keypad_scan_row(COL_3_GPIO_Port, COL_3_Pin);
			key_pressed = keypad_chars[row - 1][3 - 1];
			break;

	case COL_4_Pin:
			row = keypad_scan_row(COL_4_GPIO_Port, COL_4_Pin);
			key_pressed = keypad_chars[row - 1][4 - 1];
			break;
	default:
		break;
	}
	return key_pressed;
}

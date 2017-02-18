#include "keyboard.h"

#include "stm32f3xx_hal.h"

#define ROW1_READ HAL_GPIO_ReadPin(Keypad_8_GPIO_Port,  Keypad_8_Pin)
#define ROW2_READ HAL_GPIO_ReadPin(Keypad_3_GPIO_Port,  Keypad_3_Pin)
#define ROW3_READ HAL_GPIO_ReadPin(Keypad_4_GPIO_Port,  Keypad_4_Pin)
#define ROW4_READ HAL_GPIO_ReadPin(Keypad_6_GPIO_Port,  Keypad_6_Pin)

#define COL1_WRITE(x) HAL_GPIO_WritePin(Keypad_7_GPIO_Port, Keypad_7_Pin,  x)
#define COL2_WRITE(x) HAL_GPIO_WritePin(Keypad_9_GPIO_Port, Keypad_9_Pin,  x)
#define COL3_WRITE(x) HAL_GPIO_WritePin(Keypad_5_GPIO_Port, Keypad_5_Pin, x)

#define KEYS_KB 12

union {

    struct {
        uint8_t b0 : 1;
        uint8_t b1 : 1;
        uint8_t b2 : 1;
        uint8_t b3 : 1;
        uint8_t b4 : 1;
        uint8_t b5 : 1;
        uint8_t b6 : 1;
        uint8_t b7 : 1;
    } bytes[4]; // tet
    uint32_t code; //code
} ScanCode;

const uint32_t ScanCodes[KEYS_KB] = {0x000F0F0E, 0x000F0E0F, 0x000E0F0F, 0x000F0F0D, 0x000F0D0F, 0x000D0F0F, \
0x000F0F0B, 0x000F0B0F, 0x000B0F0F, 0x000F0F07, 0x000F070F, 0x00070F0F};
const uint8_t keyCodes[KEYS_KB] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '0', '#'};

KBD Keyboard = {0, 0};

uint8_t ReturnKey(uint32_t Scan) { //return alphabet symbol instead of code
    uint8_t i;
    for (i = 0; i < KEYS_KB; i++) {
        if (Scan == ScanCodes[i])
            return keyCodes[i];
    }
    return 0;
}

void ReadKeyboardValue(uint8_t Line) {
    ScanCode.bytes[Line].b0 = ROW1_READ;
    ScanCode.bytes[Line].b1 = ROW2_READ;
    ScanCode.bytes[Line].b2 = ROW3_READ;
    ScanCode.bytes[Line].b3 = ROW4_READ;
}

void SolveKeyboard(void) {
    static uint32_t OldKey = 0x000F0F0F;
    static uint16_t CounterK = 0; // for how long pressed the button

    if (ScanCode.code != 0x000F0F0F) {
        if (ScanCode.code != OldKey) {
            OldKey = ScanCode.code;
            CounterK = 0;
        } else {
            CounterK++;
        }

        if (CounterK == 3) {
            CounterK++;
            Keyboard.keyCode = ReturnKey(ScanCode.code);
            ScanCode.code = 0x00000000;
            if (Keyboard.keyCode)
                Keyboard.keyReady = 1;
        }
    } else {
        OldKey = 0x000F0F0F;
    }

}

void WriteKeyboardLines(uint8_t Line) {
    COL1_WRITE(GPIO_PIN_SET);
    COL2_WRITE(GPIO_PIN_SET);
    COL3_WRITE(GPIO_PIN_SET);

    switch (Line) {
        case 0:
            COL1_WRITE(GPIO_PIN_RESET);
            break;
        case 1:
            COL2_WRITE(GPIO_PIN_RESET);
            break;
        case 2:
            COL3_WRITE(GPIO_PIN_RESET);
            break;

    }
    return;
}

void ScanKeyboard(void) {
    static uint16_t SM_Counter = 0; //the state machine counter
    switch (SM_Counter) {
        case 0: WriteKeyboardLines(0);
            break;
        case 5: ReadKeyboardValue(0);
            break;
        case 10: WriteKeyboardLines(1);
            break;
        case 15: ReadKeyboardValue(1);
            break;
        case 20: WriteKeyboardLines(2);
            break;
        case 25: ReadKeyboardValue(2);
            break;
        case 30: SolveKeyboard();
            break;

    }

    SM_Counter++;
    SM_Counter %= 40;
}

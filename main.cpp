#include "hal.h"

int LED[] = {GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_11, GPIO_PIN_12};
int SW[] = {GPIO_PIN_4, GPIO_PIN_8, GPIO_PIN_10, GPIO_PIN_12};

const int DELAY = 500;
const int DELAY_FACTOR = 250;
const int LED_LENGTH = 8;
const int SW_LENGTH = 4;
const int MID = 8;
const int MAX = 14;

int frame = 1;

void TIM6_IRQ_Handler();


int umain() {

    char curSw[] = "----";
    bool swChanged = false;
    unsigned long curDelayFactor;
    registerTIM6_IRQHandler(TIM6_IRQ_Handler);

    __enable_irq();

    do {

        HAL_Delay(1);

        for (int i = 0; i < SW_LENGTH; i++) {
            char state = HAL_GPIO_ReadPin(GPIOE, SW[i]) == GPIO_PIN_SET ? '1' : '0';
            if (curSw[i] != state) {
                curSw[i] = state;
                swChanged = true;
            }
        }

        if (swChanged) {

            __disable_irq();

            curDelayFactor = std::stoul(curSw, nullptr, 2);

            WRITE_REG(TIM6_ARR, DELAY + curDelayFactor * DELAY_FACTOR);
            WRITE_REG(TIM6_DIER, TIM_DIER_UIE);
            WRITE_REG(TIM6_PSC, 0);

            WRITE_REG(TIM6_CR1, TIM_CR1_CEN);

            __enable_irq();
        }


        swChanged = false;

    } while (true);

    return 0;

}

void TIM6_IRQ_Handler() {

    int tmp = frame;
    tmp = tmp > MID ? tmp - 2 * (tmp - MID) : tmp;

    for (int i = 0; i < LED_LENGTH; i++) {
        if (i < tmp) {
            HAL_GPIO_WritePin(GPIOD, LED[i], GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(GPIOD, LED[i], GPIO_PIN_RESET);
        }
    }

    frame = frame >= MAX ? 1 : frame + 1;

}

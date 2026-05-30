#ifndef ROBOT_GPIO_H
#define ROBOT_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

// Инициализация (опционально, можно ничего не делать)
void robot_gpio_init(void);

// Команды движения
void robot_forward(void);
void robot_left(void);
void robot_right(void);
void robot_stop(void);

// Освобождение ресурсов (при завершении)
void robot_gpio_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif
#include <linux/gpio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "robot_gpio.h"

// ---------- Вспомогательная функция записи в GPIO (без задержки) ----------
static void gpio_write_pin(const char *dev_name, int offset, uint8_t value)
{
    struct gpiohandle_request rq;
    struct gpiohandle_data data;
    int fd, ret;

    fd = open(dev_name, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Не удалось открыть %s: %s\n", dev_name, strerror(errno));
        return;
    }

    rq.lineoffsets[0] = offset;
    rq.flags = GPIOHANDLE_REQUEST_OUTPUT;
    rq.lines = 1;
    ret = ioctl(fd, GPIO_GET_LINEHANDLE_IOCTL, &rq);
    close(fd);
    if (ret == -1) {
        fprintf(stderr, "Ошибка получения дескриптора линии: %s\n", strerror(errno));
        return;
    }

    data.values[0] = value;
    ret = ioctl(rq.fd, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &data);
    if (ret == -1)
        fprintf(stderr, "Не удалось установить значение линии: %s\n", strerror(errno));

    close(rq.fd);
}


static const char *GPIO_CHIP = "/dev/gpiochip0";
#define PIN_IN1 12
#define PIN_IN2 13
#define PIN_IN3 20
#define PIN_IN4 21
#define PIN_ENA 6
#define PIN_ENB 26

void robot_gpio_init(void) {
}

void robot_forward(void) {
    gpio_write_pin(GPIO_CHIP, PIN_IN1, 1);
    gpio_write_pin(GPIO_CHIP, PIN_IN2, 0);
    gpio_write_pin(GPIO_CHIP, PIN_IN3, 0);
    gpio_write_pin(GPIO_CHIP, PIN_IN4, 1);
    gpio_write_pin(GPIO_CHIP, PIN_ENA, 1);
    gpio_write_pin(GPIO_CHIP, PIN_ENB, 1);
}

void robot_left(void) {
    gpio_write_pin(GPIO_CHIP, PIN_IN1, 0);
    gpio_write_pin(GPIO_CHIP, PIN_IN2, 1);
    gpio_write_pin(GPIO_CHIP, PIN_IN3, 0);
    gpio_write_pin(GPIO_CHIP, PIN_IN4, 1);
    gpio_write_pin(GPIO_CHIP, PIN_ENA, 1);
    gpio_write_pin(GPIO_CHIP, PIN_ENB, 1);
}

void robot_right(void) {
    gpio_write_pin(GPIO_CHIP, PIN_IN1, 1);
    gpio_write_pin(GPIO_CHIP, PIN_IN2, 0);
    gpio_write_pin(GPIO_CHIP, PIN_IN3, 1);
    gpio_write_pin(GPIO_CHIP, PIN_IN4, 0);
    gpio_write_pin(GPIO_CHIP, PIN_ENA, 1);
    gpio_write_pin(GPIO_CHIP, PIN_ENB, 1);
}

void robot_stop(void) {
    gpio_write_pin(GPIO_CHIP, PIN_IN1, 0);
    gpio_write_pin(GPIO_CHIP, PIN_IN2, 0);
    gpio_write_pin(GPIO_CHIP, PIN_IN3, 0);
    gpio_write_pin(GPIO_CHIP, PIN_IN4, 0);
    gpio_write_pin(GPIO_CHIP, PIN_ENA, 1);
    gpio_write_pin(GPIO_CHIP, PIN_ENB, 1);
}

void robot_gpio_cleanup(void) {
    robot_stop();
}
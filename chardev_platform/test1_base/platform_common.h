#ifndef PLATFORM_COMMON_H
#define PLATFORM_COMMON_H
//自定义led1、led2的设备附加数据结构体
struct s5pv210_led_platdata {
        unsigned int             gpio;
        unsigned int             flags;

        char                    *name;
        char                    *def_trigger;
};
#endif

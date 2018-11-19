#ifndef _GLOBAL_
#define _GLOBAL_
//include
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "stm32f0xx_i2c.h"
#include <string.h>
#include <stdlib.h> // for strtoul()

 void nano_wait(int t);
void I2C_config(void);
void init_usart2(void);
void writechar(char );
char readchar(void);
void sendmsg(char*,int );
int print(const char *);
int println(const char *);
int readln(void *, int );
int i2c_checknack(void);


//global variable
#endif


#include "miniproject.h"
void init_usart2(void) {
	RCC->AHBENR |=RCC_AHBENR_GPIOAEN;
	GPIOA -> MODER |= (0b1010<<4);
	GPIOA -> AFR[0] &= ~0xf;
	GPIOA -> AFR[0] |= 0b1<<8 | 0b1<<12;
	GPIOA ->OSPEEDR |= 0b1111<<4 ;
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	USART2->CR1 &= ~USART_CR1_UE;
	USART2->CR1 |= (1<<28); //10=7
	USART2->CR1 &= ~(1<<12);
	USART2->CR2 &= ~USART_CR2_STOP;
	USART2->CR1 &= ~USART_CR1_PCE;
	USART2->CR1 &= ~USART_CR1_PS;//even
	USART2->CR1 &= ~USART_CR1_OVER8;//16x oversampling
	USART2->BRR = 0x1a1;
	USART2->CR1 |= USART_CR1_TE|USART_CR1_RE|USART_CR1_UE;
}

void writechar(char c) {
    while ((USART2 -> ISR & USART_ISR_TXE) != USART_ISR_TXE);
        if (c == '\n'){
            USART2 -> TDR = '\r';
        }
    while ((USART2 -> ISR & USART_ISR_TXE) != USART_ISR_TXE);
        USART2 -> TDR = c;
}

char readchar(void) {
    char c;
    if (USART2 -> ISR & USART_ISR_ORE){
        USART2 -> ICR |= USART_ICR_ORECF;
    }
    while (!(USART2 -> ISR & USART_ISR_RXNE));
    c = USART2 -> RDR;
    return c;
}


//============================================================================
// repeat_write()
// Continually write, to the serial output, three alphabets on each line.
//

void sendmsg(char* msg,int len){
	int i=0;
	for(i=0;i<len;i++){
		writechar(msg[i]);
	}
	writechar('\n');
}
//============================================================================
// Continually read characters from the serial port and write them back.


//============================================================================
// print()
// Write a string to the serial port output.
// Return the length of the string.
//
int print(const char *s) {
    const char *ptr;
    for(ptr=s; *ptr; ptr+=1)
        writechar(*ptr);
    return ptr - s;
}
int println(const char *s) {
    int len = print(s);
    print("\n");
    return len;
}

//============================================================================
// readln()
// Read a line of input.  Echo characters.  Handle backspace and printing
// of control characters.
//
int readln(void *v, int len) {
    int i;
    char *c = v;
    for(i=0; i<len; i++) {
        char tmp = readchar();
        if (tmp == '\b' || tmp == '\177') {
            if (i == 0) {
                i -= 1;
            } else {
                if (c[i-1] < 32) {
                    print("\b\b  \b\b");
                } else {
                    print("\b \b");
                }
                i -= 2;
            }
            continue;
        }
        else if (tmp == '\r') {
            print("\r\n");
            c[i++] = '\n';
            return i;
        } else if (tmp == 0) {
            print("^0");
        } else if (tmp == 28) {
            print("^\\");
        } else if (tmp < 32) {
            writechar('^');
            writechar('A'-1+tmp);
        } else {
            writechar(tmp);
        }
        c[i] = tmp;
    }
    return i;
}

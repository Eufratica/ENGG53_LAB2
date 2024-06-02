#include <xc.h>
#include <plib.h>

// Configura��o dos clock principal para 80 MHz
#pragma config FNOSC = PRIPLL // Sele��o do oscilador
#pragma config POSCMOD = HS // Modo do oscilador prim�rio
#pragma config FPLLIDIV = DIV_2 // Divisor de entrada do PLL
#pragma config FPLLMUL = MUL_20 // Multiplicador do PLL
#pragma config FPLLODIV = DIV_1 // Divisor de sa�da do PLL
#pragma config FPBDIV = DIV_1 // Divisor do clock do barramento perif�rico
#pragma config FWDTEN = OFF // Timer do watchdog desativado

#define LED1_PIN LATAbits.LATA0     // Define o LED1 no pino RA0
#define LED2_PIN LATAbits.LATA2     // Define o LED2 no pino RA2
#define LED3_PIN LATAbits.LATA3     // Define o LED3 no pino RA3

#define BUZZER_PIN LATAbits.LATA1   // Define o buzzer no pino RA1
#define BUTTON1_PIN PORTFbits.RF8   // Define o bot�o 1 no pino RF8 ### Corrigir para porta do pino 72 (INT0)
#define BUTTON2_PIN PORTFbits.RF3   // Define o bot�o 2 no pino RF3 ### Corrigir para porta do pino 18 (INT1)
#define BUTTON3_PIN PORTFbits.RF5   // Define o bot�o 3 no pino RF4 ### Corrigir para porta do pino 19 (INT2)

#define ADC_PIN 0                   // Define o pino ADC no pino AN0

#define SYS_FREQ (80000000L)        // Frequ�ncia do sistema em 32 MHz
#define VOLTAGE_THRESHOLD 512       // Limite de tens�o para acionar o LED2 e o buzzer

void delayMs(int ms) {
    // Configura o TIMER1
    T1CON = 0x8000; // Liga o TIMER1, prescaler 1:1
    TMR1 = 0; // Limpa o contador

    while (ms > 0) {
        // Configura o TIMER1 para contar 1ms
        PR1 = (SYS_FREQ / 2) / 1000 - 1;
        TMR1 = 0; // Limpa o contador
        IFS0bits.T1IF = 0; // Limpa a flag de interrup��o do TIMER1

        // Aguarda at� que o TIMER1 conte 1ms
        while (IFS0bits.T1IF == 0);

        ms--; // Decrementa a contagem de ms
    }
}

void __delay_ms(unsigned int ms) {
    while (ms--) {
        delayMs(1);  // 1 ms delay, delayMs � uma fun��o do XC32 para um atraso de 1 ms
    }
}

void setup() {
    TRISAbits.TRISA0 = 0;  // Configura o pino do LED1 como sa�da
    TRISAbits.TRISA2 = 0;  // Configura o pino do LED2 como sa�da
    TRISAbits.TRISA3 = 0;  // Configura o pino do LED3 como sa�da
    
    TRISAbits.TRISA1 = 0;  // Configura o pino do buzzer como sa�da
    
    TRISFbits.TRISF8 = 1;  // Configura o pino do bot�o 1 como entrada
    TRISFbits.TRISF3 = 1;  // Configura o pino do bot�o 2 como entrada
    TRISFbits.TRISF5 = 1;  // Configura o pino do bot�o 3 como entrada

    LED1_PIN = 0;  // Desliga o LED1
    LED2_PIN = 0;  // Desliga o LED2
    LED3_PIN = 0;  // Desliga o LED3
    BUZZER_PIN = 0;  // Desliga o buzzer

    // Configurar ADC para opera��o autom�tica com interrup��es
    AD1CON1 = 0x04E0;  // Configurar ADC para opera��o autom�tica com interrup��es
    AD1CHS = ADC_PIN;  // Selecionar o pino ADC
    AD1CSSL = 0;
    AD1CON3 = 0x1F02;  // Configurar ADC para amostragem manual
    AD1CON2 = 0;
    AD1CON1bits.ADON = 1;  // Habilitar ADC

    // Habilitar interrup��o ADC
    IFS1bits.AD1IF = 0;  // Limpar flag de interrup��o ADC
    IEC1bits.AD1IE = 1;  // Habilitar interrup��o ADC

    // Configurar INT0
    INTCONbits.INT0EP = 1;  // Interrup��o na borda de subida
    IFS0bits.INT0IF = 0;     // Limpar flag de interrup��o
    IEC0bits.INT0IE = 1;     // Habilitar interrup��o

    // Configurar INT1
    INTCONbits.INT1EP = 1;  // Interrup��o na borda de subida
    IFS0bits.INT1IF = 0;     // Limpar flag de interrup��o
    IEC0bits.INT1IE = 1;     // Habilitar interrup��o

    // Configurar INT2
    INTCONbits.INT2EP = 1;  // Interrup��o na borda de subida
    IFS0bits.INT2IF = 0;     // Limpar flag de interrup��o
    IEC0bits.INT2IE = 1;     // Habilitar interrup��o

    INTEnableSystemMultiVectoredInt();  // Habilitar interrup��es globais
}

volatile int led1_on = 0;
volatile int led2_on = 0;
volatile int led3_on = 0;
volatile int buzzer_on = 0;

void __ISR(_EXTERNAL_0_VECTOR, IPL7SRS) _INT0Interrupt(void) {
    IFS0bits.INT0IF = 0;  // Limpar flag de interrup��o

    if (led1_on && buzzer_on) {
        LED1_PIN = 0;  // Desliga o LED1
        BUZZER_PIN = 0;  // Desliga o buzzer
        led1_on = 0;
        buzzer_on = 0;
    } else if (led1_on) {
        LED1_PIN = 0;  // Desliga o LED1
        BUZZER_PIN = 0;  // Desliga o buzzer
        led1_on = 0;
        buzzer_on = 0;
    } else {
        LED1_PIN = 1;  // Liga o LED1
        BUZZER_PIN = 1;  // Liga o buzzer
        led1_on = 1;
        buzzer_on = 1;
    }
    __delay_ms(200);  // Debounce do bot�o
}

void __ISR(_EXTERNAL_1_VECTOR, IPL7SRS) _INT1Interrupt(void) {
    IFS0bits.INT1IF = 0;  // Limpar flag de interrup��o

    BUZZER_PIN = 0;  // Desliga o buzzer
    buzzer_on = 0;
    __delay_ms(200);  // Debounce do bot�o
}

void __ISR(_EXTERNAL_2_VECTOR, IPL7SRS) _INT2Interrupt(void) {
    IFS0bits.INT2IF = 0;  // Limpar flag de interrup��o

    if (led3_on && buzzer_on) {
        LED3_PIN = 0;  // Desliga o LED1
        BUZZER_PIN = 0;  // Desliga o buzzer
        led3_on = 0;
        buzzer_on = 0;
    } else if (led3_on) {
        LED3_PIN = 0;  // Desliga o LED1
        BUZZER_PIN = 0;  // Desliga o buzzer
        led3_on = 0;
        buzzer_on = 0;
    } else {
        LED3_PIN = 1;  // Liga o LED1
        BUZZER_PIN = 1;  // Liga o buzzer
        led3_on = 1;
        buzzer_on = 1;
    }
    __delay_ms(200);  // Debounce do bot�o
}

void __ISR(_ADC_VECTOR, IPL7SRS) _ADC1Interrupt(void) {
    IFS1bits.AD1IF = 0;  // Limpar flag de interrup��o ADC

    if (ADC1BUF0 > VOLTAGE_THRESHOLD) {
        
        if (!led2_on && !buzzer_on) {
            LED2_PIN = 1;  // Liga o LED2
            BUZZER_PIN = 1;  // Liga o buzzer
            led2_on = 1;
            buzzer_on = 1;
        } else if (led2_on && !buzzer_on) {
            LED2_PIN = 1;  // Liga o LED2
            BUZZER_PIN = 0;  // Desliga o buzzer
            led2_on = 1;
            buzzer_on = 0;
        }
    } else {
        
        if (led2_on && buzzer_on) {
            LED2_PIN = 0;  // Desliga o LED2
            BUZZER_PIN = 0;  // Desliga o buzzer
            led2_on = 0;
            buzzer_on = 0;
        } else if (led2_on) {
            LED2_PIN = 0;  // Desliga o LED2
            BUZZER_PIN = 0;  // Desliga o buzzer
            led2_on = 0;
            buzzer_on = 0;
        }
    }
}

int main() {
    setup();

    while (1) {
        AD1CON1bits.SAMP = 1;  // Iniciar amostragem
        __delay_ms(1);  // Aguardar 1 ms
        AD1CON1bits.SAMP = 0;  // Iniciar convers�o

        while (!AD1CON1bits.DONE);  // Aguardar a convers�o terminar
    }

    return 0;
}
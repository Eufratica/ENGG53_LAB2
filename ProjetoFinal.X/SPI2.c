#include <xc.h>
#include <stdint.h>

// Defini��es de macros e configura��es do SPI
#define SPI2_DUMMY_DATA 0xFF
#define SPI2_FIFO_FILL_LIMIT 1

typedef enum {
    SPI2_TRANSFER_MODE_32BIT = 2,
    SPI2_TRANSFER_MODE_16BIT = 1,
    SPI2_TRANSFER_MODE_8BIT = 0
} SPI2_TRANSFER_MODE;

void SPI2_Initialize(void);
void SPI2_Send(uint8_t data);
void SPI2_Exchange(uint8_t *pTransmitData, uint8_t *pReceiveData);
uint16_t SPI2_ExchangeBuffer(uint8_t *pTransmitData, uint16_t byteCount, uint8_t *pReceiveData);
uint8_t SPI2_Exchange8bit(uint8_t data);
uint16_t SPI2_Exchange8bitBuffer(uint8_t *dataTransmitted, uint16_t byteCount, uint8_t *dataReceived);
SPI2_TRANSFER_MODE SPI2_TransferModeGet(void);
uint32_t SPI2_StatusGet(void);

// Inicializa��o do SPI2
void SPI2_Initialize(void)
{
    // Desativa o m�dulo SPI2
    SPI2CONbits.ON = 0;

    // Configure SPI2 Control Register
    SPI2CONbits.MSTEN = 1;  // Modo Mestre
    SPI2CONbits.CKE = 1;    // Transmitir dados na transi��o de alta para baixa (modo 0,0)
    SPI2CONbits.CKP = 0;    // Clock idle em baixa
    SPI2CONbits.SMP = 1;    // Sample data at end of output time
    SPI2CONbits.MODE16 = 0; // Modo de comunica��o 8 bits

    // Baud Rate (Fp = 80MHz, desejado = 1MHz) SPI Clock = Fp/2*(BRG + 1)
    SPI2BRG = 39;           // Configura��o para 1MHz (Baud Rate = Fpb / (2 * (BRG + 1)))

    // Habilita SPI2 Module
    SPI2CONbits.ON = 1;
}

// Fun��o para enviar um byte via SPI2
void SPI2_Send(uint8_t data)
{
    while (SPI2STATbits.SPITBE == 0); // Aguarda at� que o buffer de transmiss�o esteja vazio

    SPI2BUF = data; // Escreve os dados no buffer de transmiss�o SPI2

    while (SPI2STATbits.SPIRBE); // Aguarda at� que a recep��o esteja completa

    uint8_t dummy = SPI2BUF; // Leitura do buffer de recep��o para limpar a flag
}

// Fun��o para trocar um byte via SPI2
void SPI2_Exchange(uint8_t *pTransmitData, uint8_t *pReceiveData)
{
    while (SPI2STATbits.SPITBE == 0); // Aguarda at� que o buffer de transmiss�o esteja vazio

    SPI2BUF = *pTransmitData; // Escreve os dados no buffer de transmiss�o SPI2

    while (SPI2STATbits.SPIRBE); // Aguarda at� que o buffer de recep��o esteja cheio

    *pReceiveData = SPI2BUF; // L� os dados do buffer de recep��o SPI2
}

// Fun��o para trocar m�ltiplos bytes via SPI2
uint16_t SPI2_ExchangeBuffer(uint8_t *pTransmitData, uint16_t byteCount, uint8_t *pReceiveData)
{
    uint16_t dataSentCount = 0;
    uint16_t dataReceivedCount = 0;
    uint8_t dummyDataReceived = 0;
    uint8_t dummyDataTransmit = (uint8_t)SPI2_DUMMY_DATA;
    uint16_t count = 0;
    uint8_t *pSend, *pReceived;

    if ((pTransmitData == NULL) && (pReceiveData == NULL))
    {
        return dataSentCount;
    }
    else if (pTransmitData == NULL)
    {
        pReceived = pReceiveData;
        while (byteCount--)
        {
            while (SPI2STATbits.SPITBE == 0);

            SPI2BUF = dummyDataTransmit;

            while (SPI2STATbits.SPIRBE);

            *pReceived++ = SPI2BUF;
        }
    }
    else if (pReceiveData == NULL)
    {
        pSend = pTransmitData;
        while (byteCount--)
        {
            while (SPI2STATbits.SPITBE == 0);

            SPI2BUF = *pSend++;

            while (SPI2STATbits.SPIRBE);

            dummyDataReceived = SPI2BUF;
        }
    }
    else
    {
        pSend = pTransmitData;
        pReceived = pReceiveData;
        while (byteCount--)
        {
            while (SPI2STATbits.SPITBE == 0);

            SPI2BUF = *pSend++;

            while (SPI2STATbits.SPIRBE);

            *pReceived++ = SPI2BUF;
        }
    }

    return dataSentCount;
}

// Fun��o para trocar um byte via SPI2 e retornar o byte recebido
uint8_t SPI2_Exchange8bit(uint8_t data)
{
    uint8_t receiveData;

    SPI2_Exchange(&data, &receiveData);

    return (receiveData);
}

// Fun��o para trocar m�ltiplos bytes via SPI2
uint16_t SPI2_Exchange8bitBuffer(uint8_t *dataTransmitted, uint16_t byteCount, uint8_t *dataReceived)
{
    return (SPI2_ExchangeBuffer(dataTransmitted, byteCount, dataReceived));
}

// Fun��o para obter o modo de transfer�ncia do SPI2
SPI2_TRANSFER_MODE SPI2_TransferModeGet(void)
{
    if (SPI2CONbits.MODE16 == 1)
        return SPI2_TRANSFER_MODE_16BIT;
    else
        return SPI2_TRANSFER_MODE_8BIT;
}

// Fun��o para obter o status do SPI2
uint32_t SPI2_StatusGet(void)
{
    return SPI2STAT;
}

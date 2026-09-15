/************************************************/
/* Layer   : HAL                                */
/* SWC     : NRF24L01(+)                        */
/* File    : NRF24_program.c                    */
/************************************************/
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "../../MCAL/RCC/RCC_interface.h"
#include "../../MCAL/GPIO/GPIO_interface.h"
#include "../../MCAL/SPI/SPI_interface.h"
#include "../../MCAL/STK/STK_interface.h"

#include "NRF24_interface.h"
#include "NRF24_private.h"
#include "NRF24_config.h"

static const u8 NRF_Au8Address[5] = NRF_ADDRESS;

/* RF_SETUP : bit5 RF_DR_LOW, bit3 RF_DR_HIGH, bits2:1 power (11 = 0 dBm) */
#if NRF_DATA_RATE == NRF_DR_250KBPS
#define NRF_RF_SETUP_VALUE   0x26
#else
#define NRF_RF_SETUP_VALUE   0x06
#endif

/* ------------------------------------------------------------------ */
/*                        Low level helpers                            */
/* ------------------------------------------------------------------ */
static void NRF_voidCeHigh(void)
{
    MGPIO_voidSetPinValue(NRF_CE_PORT, NRF_CE_PIN, GPIO_HIGH);
}

static void NRF_voidCeLow(void)
{
    MGPIO_voidSetPinValue(NRF_CE_PORT, NRF_CE_PIN, GPIO_LOW);
}

static void NRF_voidCsnHigh(void)
{
    MGPIO_voidSetPinValue(NRF_CSN_PORT, NRF_CSN_PIN, GPIO_HIGH);
}

static void NRF_voidCsnLow(void)
{
    MGPIO_voidSetPinValue(NRF_CSN_PORT, NRF_CSN_PIN, GPIO_LOW);
}

static void NRF_voidWriteReg(u8 Copy_u8Reg, u8 Copy_u8Value)
{
    NRF_voidCsnLow();
    (void)MSPI1_u8Transceive(NRF_CMD_W_REGISTER | (Copy_u8Reg & 0x1F));
    (void)MSPI1_u8Transceive(Copy_u8Value);
    NRF_voidCsnHigh();
}

static void NRF_voidWriteRegMulti(u8 Copy_u8Reg, const u8* Copy_pu8Data, u8 Copy_u8Len)
{
    u8 Local_u8Idx;

    NRF_voidCsnLow();
    (void)MSPI1_u8Transceive(NRF_CMD_W_REGISTER | (Copy_u8Reg & 0x1F));
    for (Local_u8Idx = 0; Local_u8Idx < Copy_u8Len; Local_u8Idx++)
    {
        (void)MSPI1_u8Transceive(Copy_pu8Data[Local_u8Idx]);
    }
    NRF_voidCsnHigh();
}

static u8 NRF_u8ReadReg(u8 Copy_u8Reg)
{
    u8 Local_u8Value;

    NRF_voidCsnLow();
    (void)MSPI1_u8Transceive(NRF_CMD_R_REGISTER | (Copy_u8Reg & 0x1F));
    Local_u8Value = MSPI1_u8Transceive(NRF_CMD_NOP);
    NRF_voidCsnHigh();

    return Local_u8Value;
}

static void NRF_voidSendCmd(u8 Copy_u8Cmd)
{
    NRF_voidCsnLow();
    (void)MSPI1_u8Transceive(Copy_u8Cmd);
    NRF_voidCsnHigh();
}

/* ------------------------------------------------------------------ */
/*                           Public API                                */
/* ------------------------------------------------------------------ */
void HNRF24_voidInit(void)
{
    /* 1- SPI bus */
    MSPI1_voidInit();

    /* 2- CE and CSN as push-pull outputs */
    MRCC_voidEnableClock(RCC_APB2, RCC_APB2_IOPA);
    MGPIO_voidSetPinMode(NRF_CE_PORT , NRF_CE_PIN , GPIO_OUTPUT_PP_10MHZ);
    MGPIO_voidSetPinMode(NRF_CSN_PORT, NRF_CSN_PIN, GPIO_OUTPUT_PP_10MHZ);

    NRF_voidCeLow();
    NRF_voidCsnHigh();

    /* 3- Let the radio finish its own power-on reset */
    MSTK_voidDelayMs(100);

    /* 4- Register configuration (radio kept powered down meanwhile) */
    NRF_voidWriteReg(NRF_REG_CONFIG    , 0x00);   /* power down            */
    NRF_voidWriteReg(NRF_REG_EN_RXADDR , 0x01);   /* enable data pipe 0    */
    NRF_voidWriteReg(NRF_REG_SETUP_AW  , 0x03);   /* 5-byte address        */
    NRF_voidWriteReg(NRF_REG_RF_CH     , NRF_RF_CHANNEL);
    NRF_voidWriteReg(NRF_REG_RF_SETUP  , NRF_RF_SETUP_VALUE);

#if NRF_AUTO_ACK == 1
    NRF_voidWriteReg(NRF_REG_EN_AA     , 0x01);   /* auto-ack on pipe 0    */
    NRF_voidWriteReg(NRF_REG_SETUP_RETR, 0x1F);   /* 500 us gap, 15 retries*/
#else
    NRF_voidWriteReg(NRF_REG_EN_AA     , 0x00);
    NRF_voidWriteReg(NRF_REG_SETUP_RETR, 0x00);
#endif

    NRF_voidWriteRegMulti(NRF_REG_RX_ADDR_P0, NRF_Au8Address, 5);
    NRF_voidWriteRegMulti(NRF_REG_TX_ADDR   , NRF_Au8Address, 5);

    NRF_voidWriteReg(NRF_REG_RX_PW_P0  , NRF_PAYLOAD_SIZE);
    NRF_voidWriteReg(NRF_REG_DYNPD     , 0x00);   /* static payload width  */
    NRF_voidWriteReg(NRF_REG_FEATURE   , 0x00);

    /* 5- Clean state */
    NRF_voidWriteReg(NRF_REG_STATUS, 0x70);       /* clear RX_DR/TX_DS/MAX_RT */
    NRF_voidSendCmd(NRF_CMD_FLUSH_TX);
    NRF_voidSendCmd(NRF_CMD_FLUSH_RX);

    MSTK_voidDelayMs(5);
}

void HNRF24_voidTxMode(void)
{
    NRF_voidCeLow();

    /* EN_CRC + CRCO(2 bytes) + PWR_UP , PRIM_RX = 0 */
    NRF_voidWriteReg(NRF_REG_CONFIG, 0x0E);
    NRF_voidWriteReg(NRF_REG_STATUS, 0x70);
    NRF_voidSendCmd(NRF_CMD_FLUSH_TX);

    MSTK_voidDelayMs(2);          /* power-up -> standby-I settling */
}

void HNRF24_voidRxMode(void)
{
    NRF_voidCeLow();

    NRF_voidWriteReg(NRF_REG_STATUS, 0x70);
    NRF_voidSendCmd(NRF_CMD_FLUSH_RX);

    /* EN_CRC + CRCO + PWR_UP + PRIM_RX = 1 */
    NRF_voidWriteReg(NRF_REG_CONFIG, 0x0F);

    MSTK_voidDelayMs(2);
    NRF_voidCeHigh();             /* stay in RX mode continuously */
    MSTK_voidDelayUs(200);        /* RX settling (130 us) */
}

u8 HNRF24_u8Transmit(const u8* Copy_pu8Data)
{
    u8  Local_u8Status;
    u8  Local_u8Idx;
    u16 Local_u16Timeout = 0;

    NRF_voidWriteReg(NRF_REG_STATUS, 0x70);
    NRF_voidSendCmd(NRF_CMD_FLUSH_TX);

    /* Load the payload */
    NRF_voidCsnLow();
    (void)MSPI1_u8Transceive(NRF_CMD_W_TX_PAYLOAD);
    for (Local_u8Idx = 0; Local_u8Idx < NRF_PAYLOAD_SIZE; Local_u8Idx++)
    {
        (void)MSPI1_u8Transceive(Copy_pu8Data[Local_u8Idx]);
    }
    NRF_voidCsnHigh();

    /* A CE pulse longer than 10 us starts the transmission */
    NRF_voidCeHigh();
    MSTK_voidDelayUs(20);
    NRF_voidCeLow();

    /* Wait for TX_DS (sent) or MAX_RT, with a safety timeout */
    do
    {
        MSTK_voidDelayUs(10);
        Local_u8Status = NRF_u8ReadReg(NRF_REG_STATUS);
        Local_u16Timeout++;
    }
    while (((Local_u8Status & 0x30) == 0) && (Local_u16Timeout < 400));

    NRF_voidWriteReg(NRF_REG_STATUS, 0x70);

    return (u8)GET_BIT(Local_u8Status, NRF_STATUS_TX_DS);
}

u8 HNRF24_u8DataAvailable(void)
{
    u8 Local_u8Fifo = NRF_u8ReadReg(NRF_REG_FIFO_STATUS);

    /* RX_EMPTY = 0 means there is at least one packet waiting */
    return (u8)(GET_BIT(Local_u8Fifo, NRF_FIFO_RX_EMPTY) == 0);
}

void HNRF24_voidReceive(u8* Copy_pu8Buffer)
{
    u8 Local_u8Idx;

    NRF_voidCsnLow();
    (void)MSPI1_u8Transceive(NRF_CMD_R_RX_PAYLOAD);
    for (Local_u8Idx = 0; Local_u8Idx < NRF_PAYLOAD_SIZE; Local_u8Idx++)
    {
        Copy_pu8Buffer[Local_u8Idx] = MSPI1_u8Transceive(NRF_CMD_NOP);
    }
    NRF_voidCsnHigh();

    MSTK_voidDelayUs(10);
    NRF_voidWriteReg(NRF_REG_STATUS, (1u << NRF_STATUS_RX_DR));
}

u8 HNRF24_u8ReadRegister(u8 Copy_u8Register)
{
    return NRF_u8ReadReg(Copy_u8Register);
}

u8 HNRF24_u8IsAlive(void)
{
    /* A brown-out resets the radio to its power-on defaults without any
     * SPI error. Reading back two registers we configured ourselves is
     * enough to detect that the module silently lost its settings.     */
    if (NRF_u8ReadReg(NRF_REG_RF_CH) != NRF_RF_CHANNEL)
    {
        return 0;
    }
    if (NRF_u8ReadReg(NRF_REG_RF_SETUP) != NRF_RF_SETUP_VALUE)
    {
        return 0;
    }
    if (GET_BIT(NRF_u8ReadReg(NRF_REG_CONFIG), NRF_CONFIG_PWR_UP) == 0)
    {
        return 0;
    }
    return 1;
}

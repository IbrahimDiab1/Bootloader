/*******************************************************************/
/* Author      : Ibrahim Diab                                      */
/* File Name   : USART.c                                           */
/* Description : Functions Implementation for USART module         */
/*******************************************************************/

#include "STM32F103C6.h"

#include "std_types.h"
#include "common_macros.h"

#include "USART_interface.h"
#include "USART_config.h"
#include "USART_private.h"


/* Initialize USART1 with the given baud rate */
void USART1_init(uint32 baudRate) 
{
    // Set CR1 register with various USART parameters
    USART1->CR1 = 
    (
        USART1_RECEIVER_ENABLE                          << 2  |
        USART1_TRANSMITTER_ENABLE                       << 3  |
        USART1_RXNE_INTERRUPT_ENABLE                    << 5  |
        USART1_TRANSMISSION_COMPLETE_INTERRUPT_ENABLE   << 6  |
        USART1_TXE_INTERRUPT_ENABLE                     << 7  |
        USART1_PARITY_SELECTION                         << 9  |
        USART1_PARITY_CONTROL_ENABLE                    << 10 |
        USART1_WORD_LENGTH                              << 12
    );
    // Set the number of stop bits for USART1
    MOD_2BIT(USART1->CR2, 12, USART1_STOP_BITS);
    // Set the baud rate for USART1
    USART1->BRR = calcBRRReg(baudRate, USART1_ID);
    // Clear the status register for USART1
    USART1->SR = 0;
    // Enable USART1 Peripheral
    SET_BIT(USART1->CR1, 13);
}


/* Initialize USART2 with the given baud rate */
void USART2_init(uint32 baudRate)
{
    // Set CR1 register with various USART parameters
    USART2->CR1 = 
    (
        USART2_RECEIVER_ENABLE                          << 2  |
        USART2_TRANSMITTER_ENABLE                       << 3  |
        USART2_RXNE_INTERRUPT_ENABLE                    << 5  |
        USART2_TRANSMISSION_COMPLETE_INTERRUPT_ENABLE   << 6  |
        USART2_TXE_INTERRUPT_ENABLE                     << 7  |
        USART2_PARITY_SELECTION                         << 9  |
        USART2_PARITY_CONTROL_ENABLE                    << 10 |
        USART2_WORD_LENGTH                              << 12
    );
    // Set the number of stop bits for USART2
    MOD_2BIT(USART2->CR2, 12, USART2_STOP_BITS);
    // Set the baud rate for USART2
    USART2->BRR = calcBRRReg(baudRate, USART2_ID);
    // Clear the status register for USART2
    USART2->SR = 0;
    // Enable USART2 Peripheral
    SET_BIT(USART2->CR1, 13);
}


/* Send a byte of data over USART1 */
void USART1_sendByte(uint8 byteToSend) 
{
    // Send the byte of data over USART2
    USART1->DR = byteToSend;
    // Wait until TC flag is set
    while(GET_BIT(USART1->SR,6) == 0);
}


/* Send a byte of data over USART2 */
void USART2_sendByte(uint8 byteToSend) 
{
    // Send the byte of data over USART2
    USART2->DR = byteToSend;
    // Wait until TC flag is set
    while(GET_BIT(USART2->SR,6) == 0);
}


/* 
 * Receive a byte through USART1
 * Check if data exist then save the data and return if data exists or not.
 */
uint8 USART1_receiveByte(uint8* byte)
{
    uint8 dataAvailableFlag = 1;
    
    if ( (GET_BIT((USART1 -> SR), 5)) == 1 )
        *byte = ((uint8)(USART1->DR));
    else
        dataAvailableFlag = 0;
    
    return dataAvailableFlag;
}


/* 
 * Receive a byte through USART2
 * Check if data exist then save the data and return if data exists or not.
 */
 uint8 USART2_receiveByte(uint8* byte)
{
    uint8 dataAvailableFlag = 1;
    
    if ( (GET_BIT((USART2 -> SR), 5)) == 1 )
        *byte = ((uint8)(USART2->DR));
    else
        dataAvailableFlag = 0;
    
    return dataAvailableFlag;
}


/* 
 * Send a null-terminated string through USART1
 * Send each character in the string and wait for transmission to complete before sending the next character
 * Stop sending when a null terminator character is encountered
 */
void USART1_sendString(uint8 *stringToSend)
{
    uint8 index = 0;
    while(1) // Loop until null terminator is encountered
    {
        USART1->DR = stringToSend[index]; // Send the character
        while(GET_BIT(USART1->SR,7) == 0); // Wait until TXE flag is set
        if(stringToSend[index] == '\0')
            break;
        index++;
    }
    while(GET_BIT(USART1->SR,6) == 0); // Wait until TC [Transmission Complete] flag is set.
}


/* 
 * Send a null-terminated string through USART2
 * Send each character in the string and wait for transmission to complete before sending the next character
 * Stop sending when a null terminator character is encountered
 */
void USART2_sendString(uint8 *stringToSend)
{
    uint8 index = 0;
    while(1) // Loop until null terminator is encountered
    {
        USART2->DR = stringToSend[index]; // Send the character
        while(GET_BIT(USART2->SR,7) == 0); // Wait until TXE flag is set
        if(stringToSend[index] == '\0')
            break;
        index++;
    }
    while(GET_BIT(USART2->SR,6) == 0); // Wait until TC [Transmission Complete] flag is set.
}


// This function receives a string of characters over USART1 and stores it in the passed array.
void USART1_receiveString(uint8 *strToReceive)
{
    uint8 index=0, byte;
    
    // Receive the first character and store it in the array.
    if( USART1_receiveByte(&byte)==1)
        strToReceive[index] = byte;

    // Keep receiving characters until the end of the string or '#' character is received.
    while(strToReceive[index] !='\0' && strToReceive[index] !='#')
    {
        index++;
        while(USART1_receiveByte(&byte)==0);
        strToReceive[index] = byte;
    }

    // Add the null terminator at the end of the received string.
    strToReceive[index] = '\0';
}



// This function receives a string of characters over USART2 and stores it in the passed array.
void USART2_receiveString(uint8 *strToReceive)
{
    uint8 index=0, byte;
    
    // Receive the first character and store it in the array.
    if( USART2_receiveByte(&byte)==1)
        strToReceive[index] = byte;

    // Keep receiving characters until the end of the string or '#' character is received.
    while(strToReceive[index] !='\0' && strToReceive[index] !='#')
    {
        index++;
        while(USART2_receiveByte(&byte)==0);
        strToReceive[index] = byte;
    }

    // Add the null terminator at the end of the received string.
    strToReceive[index] = '\0';

}



/* Calculate the BRR register value for a given baud rate */
uint16 calcBRRReg(uint32 baudRate, uint8 USARTX)
{
    uint16 mantissa;
    float32 fraction, div;
    
    /* Calculate the divisor based on the system clock and baud rate */
    switch(USARTX)
    {
        case USART1_ID:
            div = RCC_APB2_CLK_FRQ / (float32)(baudRate << 4); //peripheral clock for USART1
        break;

        case USART2_ID:
            div = RCC_APB1_CLK_FRQ / (float32)(baudRate << 4); //peripheral clock for USART2
        break;
    }
    /* Extract the integer part of the divisor as the mantissa */
    mantissa = div;
    
    /* Extract the fractional part of the divisor as the fraction and round it to the nearest integer */
    fraction = (uint32)((div - mantissa) * 16 + 0.5);
    
    /* Add the integer part of the fraction to the mantissa and adjust the fraction for the BRR register format */
    mantissa += ((uint32)fraction >> 4);
    fraction  =  (uint32)fraction & 0xf;
    
    /* Combine the mantissa and fraction into the BRR register value and return it */
    return ((uint16)((mantissa << 4) | (uint32)fraction));
}


/*************************************************************************/
/* Authors     : Ibrahim Diab                                            */
/* File Name   : Bootloader.c                                            */
/* Description : The main code for Bootloader                            */
/*************************************************************************/


#include "std_types.h"
#include "common_macros.h"

#include "RCC_interface.h"
#include "DIO_interface.h"
#include "USART_interface.h"
#include "STK_interface.h"
#include "FPEC_interface.h"

/* ENABLED - DISABLED */
#define VERIFYING_CODE_STATE      DISABLED

#define BOOTLOADER_TIMEOUT        10 // In second. 
#define NUM_APP_PAGES             60 
#define PAGE_0_BASE_ADDRESS       4
#define SCB_VTOR                  0x08001000
#define STACK_OFFSET              0x00000004

typedef void(*Function_t)(void);

void Parser_ParseRecord(uint8* DataBuffer);
uint8 Parser_verifyRecord(uint8* DataBuffer);

uint8 Timeout, dataCounter, AppErasingDoneFlag;
uint8 data[100];

Function_t App = NULL;

void AppPtr(void)
{
    App = *(Function_t*)(SCB_VTOR + STACK_OFFSET);
    
    if(App!= NULL)
        App();
}

void main(void)
{
    RCC_initSysClk();
    
    RCC_enableClk (RCC_APB2, RCC_GPIOA);
    RCC_enableClk (RCC_AHB , RCC_FLITF);
    RCC_enableClk (RCC_APB2, RCC_UART1);
    
    DIO_setPinDirection(DIO_GPIOA, DIO_PIN9 , OUTPUT_SPEED_2MHZ_ALPP);
    DIO_setPinDirection(DIO_GPIOA, DIO_PIN10, INPUT_FLOATING);
    
    STK_init();
    FPEC_init();
    USART1_init(9600);
    
    STK_setIntervalSingle((BOOTLOADER_TIMEOUT * 1000000), AppPtr);
    
    while(Timeout==0)
    {
        if(USART1_receiveByte(&data[dataCounter])==1)
        {
            STK_stopInterval();
            
            USART1_receiveByte(&data[dataCounter]);
            
            if(data[dataCounter]=='\n')
            {
                if(AppErasingDoneFlag == 0)
                {
                    FPEC_eraseAppArea(NUM_APP_PAGES, PAGE_0_BASE_ADDRESS);
                    AppErasingDoneFlag = 1;
                }
                
                dataCounter=0;
                Parser_ParseRecord(data);
                
                #if VERIFYING_CODE_STATE == ENABLED
                
                     if(Parser_verifyRecord(data) == 1)
                            USART1_sendString("ok");
                     else
                            USART1_sendString("err");
                #else
                    USART1_sendString("ok");
                #endif
            }
            else
                dataCounter++;
            
            STK_setIntervalSingle((BOOTLOADER_TIMEOUT * 1000000), AppPtr);
        }
        else
        {
            //Do nothing
        }   
    }   
}

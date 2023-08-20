/****************************************************************************/
/* Authors     : Ibrahim Diab                                               */
/* File Name   : Parse.c                                                    */
/* Description : A required Code for Bootloader that parse Hex file Records */                                   
/****************************************************************************/


#include "std_types.h"
#include "common_macros.h"

#include "FPEC_interface.h"

uint16 Data[100];
uint32 Address;

uint8 ConvertAsciToHex(uint8 Asci)
{
    uint8 Hex_value;
    if(Asci<58 && Asci>47)
        Hex_value = Asci-48;
    
    else if(Asci<71 && Asci>64)
        Hex_value = Asci -55;       // as A=65 then hex_value should equal 10 so we subtract 55 form Asci value.   
    
    return Hex_value;
}

void ParseData(uint8* DataBuffer)
{
    uint8 DataHexDigitValue      [4] = {0};
    uint8 AddressHexDigitValue   [4] = {0};
    uint8 DataLengthHexDigit[2] = {0};
    uint8 DataLength = 0;
    
    AddressHexDigitValue[0] = ConvertAsciToHex(DataBuffer[6]);
    AddressHexDigitValue[1] = ConvertAsciToHex(DataBuffer[5]);
    AddressHexDigitValue[2] = ConvertAsciToHex(DataBuffer[4]);
    AddressHexDigitValue[3] = ConvertAsciToHex(DataBuffer[3]);
    
    Address &= (uint32)0xFFFF0000;

    Address = Address | (uint32)(AddressHexDigitValue[0] << 0 )  |
                        (uint32)(AddressHexDigitValue[1] << 4 )  |
                        (uint32)(AddressHexDigitValue[2] << 8 )  |
                        (uint32)(AddressHexDigitValue[4] << 12)  ;
                        
    DataLengthHexDigit[0] = ConvertAsciToHex(DataBuffer[2]);
    DataLengthHexDigit[1] = ConvertAsciToHex(DataBuffer[1]);
    
    DataLength = DataLengthHexDigit[0] | (DataLengthHexDigit[1] << 4);
    
    
    for(uint8 counter = 0; counter < (DataLength/2); counter++)
    {       
        DataHexDigitValue[0] = ConvertAsciToHex(DataBuffer[((4*counter)+ 9)]);
        DataHexDigitValue[1] = ConvertAsciToHex(DataBuffer[((4*counter)+10)]);
        DataHexDigitValue[2] = ConvertAsciToHex(DataBuffer[((4*counter)+11)]);
        DataHexDigitValue[3] = ConvertAsciToHex(DataBuffer[((4*counter)+12)]);
    
    // As STM32F103 is little endian as default configuration.
        Data[counter]=
                         (DataHexDigitValue[0]<< 4) |
                         (DataHexDigitValue[1]<< 0) |
                         (DataHexDigitValue[2]<<12) |
                         (DataHexDigitValue[3]<< 8) ;
    }
    
    FPEC_writeFlash( Address, Data, (DataLength/2));
}

// The Function is responsible to handle changing upper Address record. 
void ParseUpperAddress(uint8* DataBuffer)
{
    uint8 AddressHexDigitValue[4] = {0};
    
    AddressHexDigitValue[0] = ConvertAsciToHex(DataBuffer[12]);
    AddressHexDigitValue[1] = ConvertAsciToHex(DataBuffer[11]);
    AddressHexDigitValue[2] = ConvertAsciToHex(DataBuffer[10]);
    AddressHexDigitValue[3] = ConvertAsciToHex(DataBuffer[ 9]);
    
    Address &= (uint32)0x0000FFFF;

    Address = Address | (uint32)(AddressHexDigitValue[0] << 16) |
                        (uint32)(AddressHexDigitValue[1] << 20) |
                        (uint32)(AddressHexDigitValue[2] << 24) |
                        (uint32)(AddressHexDigitValue[4] << 28) ;
}

void Parser_ParseRecord(uint8* DataBuffer)
{

    switch(DataBuffer[8])
    {
        case 0: ParseData(DataBuffer);         break;
        case 4: ParseUpperAddress(DataBuffer); break;
        case 5: //Implement function to Parse full Address
        case 1: //End of file
        break;
    }
    
}

uint8 Parser_verifyRecord(uint8* DataBuffer)
{
    uint8 ByteHexDigit[2] = {0};
    uint8 ByteValue = 0, DigitsLength = 0;
    uint8 sum=0;
    
    ByteHexDigit[0] = ConvertAsciToHex(DataBuffer[2]);
    ByteHexDigit[1] = ConvertAsciToHex(DataBuffer[1]);
    
    ByteValue = ByteHexDigit[0] | (ByteHexDigit[1] << 4);               //Get bytes number in whole record
    DigitsLength = (ByteValue + 4)*2;
    sum = ByteValue;
    
    for(uint8 counter=3; counter < DigitsLength; counter+=2)            //Calculate the sum of bytes
    {
        ByteHexDigit[0] = ConvertAsciToHex(DataBuffer[counter]);
        ByteHexDigit[1] = ConvertAsciToHex(DataBuffer[counter + 1]);
        
        ByteValue = ByteHexDigit[1] | (ByteHexDigit[0] << 4);
        sum+= ByteValue;
    }
    
    ByteHexDigit[0] = ConvertAsciToHex(DataBuffer[DigitsLength+1]);
    ByteHexDigit[1] = ConvertAsciToHex(DataBuffer[DigitsLength+2]);
        
    ByteValue = ByteHexDigit[1] | (ByteHexDigit[0] << 4);               //Get check Sum value
    
    sum = ~sum;                                                         //Convert to two's complement
    sum++;
    
    if(sum == ByteValue)
        return 1;

    return 0;
}

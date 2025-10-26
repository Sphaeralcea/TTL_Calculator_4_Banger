#define ADR0 2
#define ADR1 3
#define ADR2 4
#define ADR3 5
#define ADR4 6
#define ADR5 7
#define ADR6 8
#define ADR7 9
#define ADR8 10
#define ADR9 11
#define ADR10 12
#define SCLK A0
#define SDOUT A1
#define CEb A2
#define OEb A3
#define WEb A4
#define LDSHb A5
#define SDIN 13

#define BLKSZ 32

void setup() {
    pinMode(ADR0, OUTPUT); digitalWrite(ADR0, LOW);
    pinMode(ADR1, OUTPUT); digitalWrite(ADR1, LOW);
    pinMode(ADR2, OUTPUT); digitalWrite(ADR2, LOW);
    pinMode(ADR3, OUTPUT); digitalWrite(ADR3, LOW);
    pinMode(ADR4, OUTPUT); digitalWrite(ADR4, LOW);
    pinMode(ADR5, OUTPUT); digitalWrite(ADR5, LOW);
    pinMode(ADR6, OUTPUT); digitalWrite(ADR6, LOW);
    pinMode(ADR7, OUTPUT); digitalWrite(ADR7, LOW);
    pinMode(ADR8, OUTPUT); digitalWrite(ADR8, LOW);
    pinMode(ADR9, OUTPUT); digitalWrite(ADR9, LOW);
    pinMode(ADR10, OUTPUT); digitalWrite(ADR10, LOW);
    pinMode(SCLK, OUTPUT); digitalWrite(SCLK, LOW);
    pinMode(SDOUT, OUTPUT); digitalWrite(SDOUT, LOW);
    pinMode(CEb, OUTPUT); digitalWrite(CEb, HIGH);
    pinMode(OEb, OUTPUT); digitalWrite(OEb, HIGH);
    pinMode(WEb, OUTPUT); digitalWrite(WEb, HIGH);
    pinMode(LDSHb, OUTPUT); digitalWrite(LDSHb, LOW);
    pinMode(SDIN, INPUT);

    Serial.begin(9600);
}

#define _HANG while(1) delay(1000)

// declare ROM image to program and ROMSZ
//#include "ROM.h"
//#include "TSTROM1.h"
#include "TSTROMDSKY.h"
//#include "TSTROMINCR.h"


//byte readRxNumBlks() {        // Block until a byte is available and return it as the block size
//     while (Serial.available() == 0)
//         delay(100);
//     return(Serial.read());
//}

//void readRxDataBlk() {                             // Read a block of program data from Serial
//    while (Serial.available() == 0)
//        delay(100);
//    delay(100);
//    for (byte i=0; i<BLKSZ; i++) {
//      pgmArray[i] = Serial.read();
//    }
//}

//byte getNumBlks() {
//    if (sizeof(pgmArray) % BLKSZ == 0)
//        return(sizeof(pgmArray) % BLKSZ);
//    else
//        return((sizeof(pgmArray) % BLKSZ)+1);
//}

void setROMAddr(int addr) {                             // Apply address to ROM address pins 
    digitalWrite(ADR0, (addr&0x0001) ? HIGH : LOW);              
    digitalWrite(ADR1, (addr&0x0002) ? HIGH : LOW);
    digitalWrite(ADR2, (addr&0x0004) ? HIGH : LOW);
    digitalWrite(ADR3, (addr&0x0008) ? HIGH : LOW);
    digitalWrite(ADR4, (addr&0x0010) ? HIGH : LOW);
    digitalWrite(ADR5, (addr&0x0020) ? HIGH : LOW);
    digitalWrite(ADR6, (addr&0x0040) ? HIGH : LOW);
    digitalWrite(ADR7, (addr&0x0080) ? HIGH : LOW);
    digitalWrite(ADR8, (addr&0x0100) ? HIGH : LOW);
    digitalWrite(ADR9, (addr&0x0200) ? HIGH : LOW);
    digitalWrite(ADR10, (addr&0x0400) ? HIGH : LOW);
}

byte getPGMByte (int addr) {
  return(pgm_read_word_near(&ROM[addr]));
}

void printArray() {
    for (int i=0; i<ROMSZ; i+=16) {
        Serial.print(i,HEX);Serial.print(": ");
        for (byte j=0; j<16; j++) {
            Serial.print(getPGMByte(i+j),HEX);
            Serial.print(" ");
        }
        Serial.println(" ");
    }
}

void pgmByte(byte outByte, int addr) { 
    byte outBit;
    setROMAddr(addr);
    //Serial.print(outByte);
    for (byte i=0; i<8; i++) {          // Serial write data to '299 shift reg
      //Serial.print(outByte);
      outBit = outByte&0x01; //Serial.print(outBit);Serial.print(" ");
      (outBit==1) ? digitalWrite(SDOUT, HIGH) : digitalWrite(SDOUT, LOW);
      delayMicroseconds(20);
      digitalWrite(SCLK, HIGH);         // '299 right shifts pgm data bit
      delayMicroseconds(50);
      digitalWrite(SCLK, LOW);
      delayMicroseconds(10);
      outByte >>= 1;
    }                                   // parallel byte is now at ROM Data pins
    digitalWrite(CEb, LOW);    // ROM chip enabled
    delayMicroseconds(50);
    digitalWrite(WEb, LOW);    // byte is programmed
    delay(9);
    digitalWrite(WEb, HIGH);
    delayMicroseconds(50);
    digitalWrite(CEb, HIGH);
}

byte readROMByte(int addr) {                         // Read ROM to '299 shift reg and serial read '299 to internal var
    byte inByte = 0;
    byte inBit = 0;

    setROMAddr(addr);
    digitalWrite(LDSHb, HIGH);  // '299 in parallel load mode
    digitalWrite(CEb, LOW);
    delayMicroseconds(50);
    digitalWrite(OEb, LOW);    // ROM is in read data out mode
    delay(1);
    digitalWrite(SCLK, HIGH);  // This edge loads ROM read data into '299
    delayMicroseconds(100);
    digitalWrite(SCLK, LOW); 
    delayMicroseconds(50);
    digitalWrite(OEb, HIGH);
    delayMicroseconds(50);
    digitalWrite(LDSHb, LOW);  // '299 is back in parralell out/shift right mode
    delayMicroseconds(100);
    for (byte i=0; i<8; i++) {
        inBit = digitalRead(SDIN);   // put SDIN value in msb of inBit.  inBit=SDIN cat B0000000
        //Serial.print(inBit);Serial.print(" ");
        if (inBit==1) inByte = (inByte>>1) | 0x80;
        else inByte = inByte>>1;
        digitalWrite(SCLK, HIGH);  // right shift the '299 one bit
        delayMicroseconds(50);
        digitalWrite(SCLK, LOW);
        delayMicroseconds(100);
    }
    return(inByte);
}

int verByte(int addr) {
    byte refbyte = getPGMByte(addr);
    byte rombyte = readROMByte(addr);

    if (refbyte != rombyte) {
        Serial.print("Fail at address "); Serial.print(addr); Serial.print(".  pgm=");Serial.print(refbyte,HEX);Serial.print(" rom=");Serial.println(rombyte,HEX);
        return(0);
    }
    else
        return(1);
}

void pgmROM() {
    for(int i=0; i < ROMSZ; i++) {
        pgmByte(getPGMByte(i), i);
    }
}

void verROM() {
    for(int i=0; i<ROMSZ; i++)
        verByte(i);
}

void loop() {
// put your main code here, to run repeatedly:
    int errors=0;

    for (int i=1; i<ROMSZ; i++) {
        Serial.println(i);
        pgmByte(getPGMByte(i), i);
        if (verByte(i)==0) errors++;
    }
//    printArray();
    Serial.print("Programming Done With ");Serial.print(errors);Serial.println(" Errors.");
    _HANG;
}

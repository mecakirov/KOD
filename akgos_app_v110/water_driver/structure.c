/* --COPYRIGHT--,BSD
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/*
 *  structure.c
 *
 *  Summary
 *     MSP430G2559 target board, custom adapter, and Capacitive Touch Booster Pack
 *
 *  Details
 *     Capacitive Touch Booster Pack: 4 element wheel, middle button, and 
 *     proximity sensor.  MSP430 harware configuration is RO_PINOSC_TA1_TB0.
 *
 *  The element and sensor definitions found in the configuration file 
 *  structure.c use designated initializer lists. This allows members to be 
 *  initialized in any order and also enhances the readability of the element
 *  being initialized.  
 *  This feature requires the GCC language extension found in Code Composer 
 *  Studio (CCS).  C99 is the default dialect found in IAR and therefore the 
 *  default settings can be used.
 */
#include "structure.h" 

// First Element (P4.7)
const struct Element pad0 = {

    .inputPxselRegister = (unsigned char *)&P4SEL,
    .inputPxsel2Register = (unsigned char *)&P4SEL2,
    .inputBits = BIT7,
    .threshold = 80
};


// Second Element (P2.1)
const struct Element pad1 = {

    .inputPxselRegister = (unsigned char *)&P2SEL,
    .inputPxsel2Register = (unsigned char *)&P2SEL2,
    .inputBits = BIT1,
    .threshold = 80
};


// Third Element (P4.6)
const struct Element pad2 = {

    .inputPxselRegister = (unsigned char *)&P4SEL,
    .inputPxsel2Register = (unsigned char *)&P4SEL2,
    .inputBits = BIT6,
    .threshold = 80
};

// Fourth Element (P2.2)
const struct Element pad3 = {

    .inputPxselRegister = (unsigned char *)&P2SEL,
    .inputPxsel2Register = (unsigned char *)&P2SEL2,
    .inputBits = BIT2,
    .threshold = 80
};


// Fifth Element (P4.5)
const struct Element pad4 = {

    .inputPxselRegister = (unsigned char *)&P4SEL,
    .inputPxsel2Register = (unsigned char *)&P4SEL2,
    .inputBits = BIT5,
    .threshold = 80
};

// Sixth Element (P3.0)
const struct Element pad5 = {

    .inputPxselRegister = (unsigned char *)&P3SEL,
    .inputPxsel2Register = (unsigned char *)&P3SEL2,
    .inputBits = BIT0,
    .threshold = 80
};

//*** CAP TOUCH HANDLER *******************************************************/
// This defines the grouping of sensors, the method to measure change in
// capacitance, and the function of the group

//PinOsc Wheel
const struct Sensor pad_sensors =
{
    .halDefinition = RO_PINOSC_TA1_TB0,
    .numElements = 6,
    .baseOffset = 0,
    // Pointer to elements
    .arrayPtr[0] = &pad0,                                  // point to pad0
    .arrayPtr[1] = &pad1,
    .arrayPtr[2] = &pad2,
    .arrayPtr[3] = &pad3,
    .arrayPtr[4] = &pad4,
    .arrayPtr[5] = &pad5,
    // Timer Information
    .sourceScale = TIMER_SOURCE_DIV_0,                     //  No divider
    .measGateSource = TIMER_ACLK,                          //  0->SMCLK, 1-> ACLK
    .accumulationCycles = 512                              //  Count for 512 cycles (at 32768HZ) = 15.625ms
};

uint16_t padCounts[6];

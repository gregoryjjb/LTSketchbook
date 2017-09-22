/*!
Linear Technology DC2028A Demonstration Board
LTC2977: Power Management Solution for Application Processors

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.

@endverbatim

http://www.linear.com/product/LTC2977

http://www.linear.com/demo/DC2028A

REVISION HISTORY
$Revision: 3033 $
$Date: 2014-12-05 14:58:30 -0800 (Fri, 05 Dec 2014) $

Copyright (c) 2015, Linear Technology Corp.(LTC)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of Linear Technology Corp.

The Linear Technology Linduino is not affiliated with the official Arduino team.
However, the Linduino is only possible because of the Arduino team's commitment
to the open-source community.  Please, visit http://www.arduino.cc and
http://store.arduino.cc , and consider a purchase that will help fund their
ongoing work.
*/

/*! @file
    @ingroup LTC2977
*/

#include <Arduino.h>
#include <stdint.h>
#include <Linduino.h>
#include <UserInterface.h>
#include <LT_Wire.h>
#include <LT_PMBus.h>
#include <LT_SMBusPec.h>
#include <LT_PMBusMath.h>
#include <LT_SMBus.h>
#include <LT_I2CBus.h>
#include <LT_SMBusGroup.h>
#include <LT_FaultLog.h>
#include <LT_SMBusNoPec.h>
#include <LT_SMBusBase.h>

#define LTC2977_I2C_ADDRESS 0x33

// Global variables
static uint8_t ltc2977_i2c_address;
static LT_SMBus *smbus = new LT_SMBusNoPec();
static LT_PMBus *pmbus = new LT_PMBus(smbus);

//! Initialize Linduino
void setup()
{
  Serial.begin(115200);         //! Initialize the serial port to the PC
  print_title();
  ltc2977_i2c_address = LTC2977_I2C_ADDRESS;
  print_prompt();
}

//! Repeats Linduino loop
//! @return void
void loop()
{
  uint8_t user_command;
  uint8_t res;
  uint8_t model[7];
  uint8_t revision[10];
  uint8_t *addresses = NULL;

  if (Serial.available())                          //! Checks for user input
  {
    user_command = read_int();                     //! Reads the user command
    if (user_command != 'm')
      Serial.println(user_command);

    switch (user_command)                          //! Prints the appropriate submenu
    {
      case 1:
        menu_1_basic_commands();                 // Print single-ended voltage menu
        break;
      case 2:
        pmbus->enablePec(ltc2977_i2c_address);
        delete smbus;
        delete pmbus;
        smbus = new LT_SMBusPec();
        pmbus = new LT_PMBus(smbus);
        break;
      case 3:
        pmbus->disablePec(ltc2977_i2c_address);
        delete smbus;
        delete pmbus;
        smbus = new LT_SMBusNoPec();
        pmbus = new LT_PMBus(smbus);
        break;
      case 4:
        addresses = smbus->probe(0);
        while (*addresses != 0)
        {
          Serial.print(F("ADDR 0x"));
          Serial.println(*addresses++, HEX);
        }
        break;
      case 5 :
        pmbus->startGroupProtocol();
        pmbus->restoreFromNvm(ltc2977_i2c_address);
        pmbus->executeGroupProtocol();
      default:
        Serial.println(F("Incorrect Option"));
        break;
    }
    print_prompt();
  }

}

// Function Definitions

//! Prints the title block when program first starts.
void print_title()
{
  Serial.print(F("\n*****************************************************************\n"));
  Serial.print(F("* DC2028A Hello World Demonstration Program                     *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates how to send and receive data from   *\n"));
  Serial.print(F("* the LTC2028A demo board.                                      *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Prints main menu.
void print_prompt()
{
  Serial.print(F("\n  1-Basic Commands\n"));
  Serial.print(F("  2-PEC On\n"));
  Serial.print(F("  3-PEC Off\n"));
  Serial.print(F("  4-Bus Probe\n"));
  Serial.print(F("  5-Reset\n"));
  Serial.print(F("\nEnter a command:"));
}

//! Prints a warning if the demo board is not detected.
void print_warning_prompt()
{
  Serial.println(F("\nWarning: Demo board not detected. Linduino will attempt to proceed."));
}

void print_all_voltages()
{
  float   voltage;
  uint8_t page;

  for (page = 0; page < 8; page++)
  {
    pmbus->setPage(ltc2977_i2c_address, page);
    voltage = pmbus->readVout(ltc2977_i2c_address, false);
    Serial.print(F("LTC2977 VOUT "));
    Serial.println(voltage, DEC);
  }

}


void print_all_status()
{
  uint8_t b;
  uint16_t w;
  uint8_t page;


  for (page = 0; page < 8; page++)
  {
    Serial.print(F("PAGE "));
    Serial.println(page, DEC);
    pmbus->setPage(ltc2977_i2c_address, page);
    b = pmbus->readStatusByte(ltc2977_i2c_address);
    Serial.print(F("LTC2977 STATUS BYTE 0x"));
    Serial.println(b, DEC);
    w = pmbus->readStatusWord(ltc2977_i2c_address);
    Serial.print(F("LTC2977 STATUS WORD 0x"));
    Serial.println(w, HEX);
  }
}

void sequence_off_on()
{
  pmbus->sequenceOffGlobal();
  delay (2000);
  pmbus->sequenceOnGlobal();
}

void margin_high()
{
  pmbus->marginHighGlobal();
}

void margin_low()
{
  pmbus->marginLowGlobal();
}

void margin_off()
{
  pmbus->sequenceOnGlobal();
}

void menu_1_basic_commands()
{
  uint8_t user_command;

  do
  {
    //! Displays the Read/Write menu
    Serial.print(F("  1-Read All Voltages\n"));
    Serial.print(F("  2-Read All Status\n"));
    Serial.print(F("  3-Sequence Off/On\n"));
    Serial.print(F("  4-Margin High\n"));
    Serial.print(F("  5-Margin Low\n"));
    Serial.print(F("  6-Margin Off\n"));
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\nEnter a command: "));

    user_command = read_int();                              //! Reads the user command
    if (user_command == 'm')                                // Print m if it is entered
    {
      Serial.print(F("m\n"));
    }
    else
      Serial.println(user_command);                         // Print user command

    switch (user_command)
    {
      case 1:
        print_all_voltages();
        break;
      case 2:
        print_all_status();
        break;
      case 3:
        sequence_off_on();
        break;
      case 4:
        margin_high();
        break;
      case 5:
        margin_low();
        break;
      case 6:
        margin_off();
        break;
      default:
        if (user_command != 'm')
          Serial.println(F("Invalid Selection"));
        break;
    }
  }
  while (user_command != 'm');
}



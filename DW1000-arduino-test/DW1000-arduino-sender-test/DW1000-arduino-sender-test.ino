/*
 * Copyright (c) 2015 by Thomas Trojer <thomas@trojer.net>
 * Decawave DW1000 library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 * Use this to test simple sender/receiver functionality with two
 * DW1000. Complements the "DW1000-arduino-receiver-test" sketch. 
 */

#include <SPI.h>
#include <DW1000.h>

// DEBUG packet sent status and count
boolean sent = false;
volatile boolean sentAck = false;
volatile unsigned long delaySent = 0;
int sentNum = 0;
unsigned long sentTime = 0;
// reset line to the chip
int RST = 9;

void setup() {
  // DEBUG monitoring
  Serial.begin(9600);
  Serial.println("### DW1000-arduino-sender-test ###");
  // initialize the driver
  DW1000.begin(0, RST);
  DW1000.select(SS);
  Serial.println("DW1000 initialized ...");
  // general configuration
  DW1000.newConfiguration();
  DW1000.setDefaults();
  DW1000.setDeviceAddress(5);
  DW1000.setNetworkId(10);
  DW1000.setFrameFilter(false);
  DW1000.commitConfiguration();
  Serial.println("Committed configuration ...");
  // DEBUG chip info and registers pretty printed
  Serial.print("Device ID: "); Serial.println(DW1000.getPrintableDeviceIdentifier());
  Serial.print("Unique ID: "); Serial.println(DW1000.getPrintableExtendedUniqueIdentifier());
  Serial.print("Network ID & Device Address: "); Serial.println(DW1000.getPrintableNetworkIdAndShortAddress());
  // attach callback for (successfully) sent messages
  DW1000.attachSentHandler(handleSent);
  // start a transmission
  transmitter();
}

void handleSent() {
  // status change on sent success
  sentAck = true;
}

void transmitter() {
  // transmit some data
  Serial.print("Transmitting packet ... #"); Serial.println(sentNum);
  DW1000.newTransmit();
  DW1000.setDefaults();
  String msg = "Hello DW1000, it's #"; msg += sentNum;
  DW1000.setData(msg);
  // delay sending the message for the given amount
  DW1000.setDelay(500, DW1000.MILLISECONDS);
  DW1000.startTransmit();
  delaySent = millis();
}

void loop() {
  if(!sentAck) {
    return;
  }
  // continue on success confirmation 
  // (we are here after the given amount of send delay time has passed)
  sentAck = false;
  // update and print some information about the sent message
  Serial.print("Delay sent [ms] ... "); Serial.println(millis() - delaySent);
  unsigned long newSentTime = DW1000.getTransmitTimestamp();
  Serial.print("Processed packet ... #"); Serial.println(sentNum);
  Serial.print("Sent timestamp ... "); Serial.println(newSentTime);
  // note: delta is just for simple demo as not correct on system time counter wrap-around
  Serial.print("Delta send time [s] ... "); Serial.println((newSentTime - sentTime) * 1.0e-6);
  sentTime = newSentTime;
  sentNum++;
  // again, transmit some data
  transmitter();
}

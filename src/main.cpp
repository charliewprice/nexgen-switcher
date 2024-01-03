#include <Arduino.h>

// Example 6 - Receiving binary data

const byte numBytes = 32;
byte receivedBytes[numBytes];
byte numReceived = 0;

boolean newData = false;

struct rs485packet {
  uint8_t  startMarker = 0x3C;
  uint8_t  command;
  uint8_t  cksum;
  uint16_t switches;
  uint8_t  endMarker = 0x3E;
};

typedef struct rs485packet Rs485Packet;
Rs485Packet rPacket;
char rsbytes[sizeof(rPacket)];

#define CMD_SENDSWITCHES 0xD3

void recvBytesWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    byte startMarker = 0x3C;
    byte endMarker = 0x3E;
    byte rb;
   

    while (Serial.available() > 0 && newData == false) {
        rb = Serial.read();

        if (recvInProgress == true) {
            if (rb != endMarker) {
                receivedBytes[ndx] = rb;
                ndx++;
                if (ndx >= numBytes) {
                    ndx = numBytes - 1;
                }
            }
            else {
                receivedBytes[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                numReceived = ndx;  // save the number for use when printing
                ndx = 0;
                newData = true;
            }
        }

        else if (rb == startMarker) {
            recvInProgress = true;
        }
    }
}

void parseSerialData() {
    if (newData == true) {
        memcpy(&rPacket, receivedBytes, sizeof(rPacket));
        if (rPacket.command == CMD_SENDSWITCHES) {
          Serial.print("This just in (HEX values)... ");
          for (byte n = 0; n < numReceived; n++) {
            Serial.print(receivedBytes[n], HEX);
            Serial.print(' ');
          }
          Serial.println();
        }
        newData = false;
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Nexgen-Switcher is ready.>");
}

void loop() {
    recvBytesWithStartEndMarkers();
    parseSerialData();
}
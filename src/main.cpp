#include <Arduino.h>
#include <IRremote.h>

const int RECEIVER_PIN = 6;
IRrecv receiver(RECEIVER_PIN);

const int SENDER_PIN = 3;
IRsend sender(SENDER_PIN);

const int SCAN_BUTTON_PIN = 11;
const int SEND_BUTTON_PIN = 12;

void setup() {
  Serial.begin(9600);

  receiver.enableIRIn();

  pinMode(SCAN_BUTTON_PIN, INPUT);
  pinMode(SEND_BUTTON_PIN, INPUT);
}

IRData scanned_info;
bool scanned_info_is_empty = true;

void scan();
void send_scanned_command();

void loop() {
  if (digitalRead(SCAN_BUTTON_PIN) == HIGH) {
    scan();
  }
  if (digitalRead(SEND_BUTTON_PIN) == HIGH) {
    send_scanned_command();
  }
  delay(1000);
}

void scan() {
  bool found_signal = false;
  for (int i = 0; i <= 10; i++) {
    Serial.print("Scanning ");
    Serial.print(i);
    Serial.println("....");
    if (receiver.decode()) {
      found_signal = true;
      break;
    }
    delay(1000);
  }
  if (found_signal) {
    Serial.println("--------------");
    Serial.print("protocol (enum decode_type_t inside IRProtocol.h): ");
    Serial.println(receiver.decodedIRData.protocol);
    scanned_info.protocol = receiver.decodedIRData.protocol;

    Serial.print("address: 0x");
    Serial.println(receiver.decodedIRData.address, HEX);
    scanned_info.address = receiver.decodedIRData.address;

    Serial.print("command: 0x");
    Serial.println(receiver.decodedIRData.command, HEX);
    scanned_info.command = receiver.decodedIRData.command;

    Serial.print("flags: 0x");
    Serial.println(receiver.decodedIRData.flags, HEX);
    scanned_info.flags = receiver.decodedIRData.flags;

    Serial.print("numberOfBits (matters only for SONY devices): ");
    Serial.println(receiver.decodedIRData.numberOfBits);
    scanned_info.numberOfBits = receiver.decodedIRData.numberOfBits;

    Serial.print("decoded raw data (matters only for MAGIQUEST devices): 0x");
    Serial.println(receiver.decodedIRData.decodedRawData, HEX);
    scanned_info.decodedRawData = receiver.decodedIRData.decodedRawData;
    scanned_info_is_empty = false;

    receiver.resume();
  }
  else {
    Serial.println("Unable to find any signal.");
  }
}

void send_scanned_command() {
  if (scanned_info_is_empty) {
    Serial.println("Nothing to send :(");
    return;
  }

  int status_code = sender.write(&scanned_info, 0);
  if (status_code == 1) {
    Serial.println("Successfully sent cloned command.");
  }
}
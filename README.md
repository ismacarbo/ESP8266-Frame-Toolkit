ESP8266 Wi-Fi Management Frame Toolkit (Educational)
====================================================

This project is an educational toolkit demonstrating how to use an ESP8266 to send low-level Wi-Fi 802.11 management frames (such as deauthentication, disassociation, and beacon frames). It is designed for cybersecurity research, penetration testing (on your own networks), and Wi-Fi protocol education.

WARNING: This code is for educational and authorized testing purposes only. Running it on networks you do not own or have permission to test may be illegal in your jurisdiction.

Features
--------

- Interactive serial interface to control attacks:
  - deauth     → Sends deauthentication frames to/from a target device
  - disassoc   → Sends disassociation frames to/from a target device
  - broadcast  → Sends deauthentication frames to the broadcast address
  - beacon     → Sends fake beacon frames with randomized SSIDs (FakeAP)

- 100 packets per burst (very high frequency)
- Fixed on channel 8 (customizable in code)
- Serial debug output for each packet sent
- Designed to work with PlatformIO and ESP8266-based boards

Requirements
------------

- ESP8266 board (NodeMCU, Wemos D1 Mini, etc.)
- PlatformIO installed in Visual Studio Code
- USB cable to flash the firmware

Usage
-----

1. Flash the code to your ESP8266 using PlatformIO.
2. Open the serial monitor at 115200 baud.
3. Type one of the following commands:
   - deauth
   - disassoc
   - broadcast
   - beacon
4. Start the attack with: start
5. Stop the attack with: stop

Example:
--------
deauth
start

Packet Monitoring (Optional)
----------------------------

To analyze the packets being sent in real time:

- Use a Wi-Fi card in monitor mode
- Recommended tools:
  - tcpdump:
      sudo tcpdump -i wlan0 -I -y IEEE802_11 subtype deauth
  - Wireshark:
      Use the display filter: wlan.fc.type_subtype == 0x0c

Project Structure
-----------------

- main.cpp: core implementation
- platformio.ini: PlatformIO configuration
- README.txt: project documentation
- LICENSE: MIT License

License
-------

MIT License

This project is provided "as is", without warranty of any kind. The author is not responsible for any misuse or damage caused by this software.

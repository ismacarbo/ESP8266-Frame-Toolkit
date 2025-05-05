from scapy.all import *
import time


devices = {}

def callback(pkt):
    if pkt.haslayer(Dot11):
        mac = pkt.addr2
        bssid = pkt.addr3
        ssid = pkt.info.decode(errors="ignore") if pkt.haslayer(Dot11Beacon) else ""

        if pkt.type == 0 and pkt.subtype == 8:
            
            if mac not in devices:
                devices[mac] = "AP"
                print(f"[AP] {mac}  SSID: {ssid}")
        elif pkt.type == 0 and pkt.subtype == 4:
            
            if mac not in devices:
                devices[mac] = "Client"
                print(f"[Client Probe] {mac}")
        elif pkt.type == 2:
            
            if mac not in devices:
                devices[mac] = "Client"
                print(f"[Client] {mac} → {bssid}")

print("scanning...")
try:
    sniff(iface="port", prn=callback, store=0)
except KeyboardInterrupt:
    print("\nscan completed")

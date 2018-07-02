# Instruccions per compilar la versio d'ESPurna utilitzada en aquest curs

1. Baixat espurna-1.13.0.
2. Descomprimeix-lo en qualsevol ubicació, p.ex. C:\kk\
3. Obre des de l’Arduino IDE l’sketch C:\kk\ espurna-1.13.0\code\espurna\espurna.ino
4. Compila per i puja per la placa nodeMCU 1.0
5. Un cop pujat el firmware, obre el serial monitor, a 115200 bps Ambdos  NL & CR.
6. Configura via serial el nou password d’administrador: `set adminPass Melda2018`
7. Configura les credencials wifi ssid: `set ssid0 IoT-2` password:  `set pass0 pitufito`
8. Reseteja el wifi de l’esp8266: `reset.wifi`
9. 

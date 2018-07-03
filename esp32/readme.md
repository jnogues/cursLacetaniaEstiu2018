# Instal·lació del suport per ESP32 a l'Arduino IDE

1. Crea una carpeta `C:\kk\arduino-1.8.5\portable\sketchbook\hardware\espressif\esp32`
2. Descarregueu el suport per [ESP32](https://codeload.github.com/espressif/arduino-esp32/zip/master) 
3. Descomprimeix el contingut en la carpeta creada.  
4. Entra a la carpeta `C:\kk\arduino-1.8.5\portable\sketchbook\hardware\espressif\esp32\tools`
5. Executa **get.exe**

![wemos32 oled](https://cdn.instructables.com/FAK/RKQ0/J3YPR3IJ/FAKRKQ0J3YPR3IJ.LARGE.jpg?auto=webp&crop=3:2)

Bus I2C per GPIO4 (SCL) i GPIO5 (SDA) amb adeça 0x3c: `SSD1306 display(0x3c, 5, 4);`

Entrada I16 per GPIO16.

Entrada I25 per GPIO25, actuant sobre Q26, GPIO26.

[Més info.](https://github.com/FablabTorino/AUG-Torino/wiki/Wemos-Lolin-board-(ESP32-with-128x64-SSD1306-I2C-OLED-display))

[Més info.](https://www.hackster.io/johnnyfrx/esp32-with-integrated-oled-wemos-lolin-getting-started-07ac5d)


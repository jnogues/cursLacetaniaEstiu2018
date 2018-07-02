# Instruccions per compilar la versio d'ESPurna utilitzada en aquest curs

1. Baixat espurna-1.13.0.
2. Descomprimeix-lo en qualsevol ubicació, p.ex. C:\kk\
3. Obre des de l’Arduino IDE l’sketch C:\kk\espurna-1.13.0\code\espurna\espurna.ino
4. Compila i puja per la placa **nodeMCU 1.0**
5. Un cop pujat el firmware, obre el serial monitor, a 115200 bps i Ambdos  NL & CR.
6. Configura via serial el nou password d’administrador: `set adminPass Melda2018`
7. Configura les credencials wifi ssid: `set ssid0 IoT-2` password:  `set pass0 pitufito`
8. Reseteja el wifi de l’esp8266: `reset.wifi`
9. Observa al serial monitor la IP assignada pel router a la nodeMCU, p.ex 192.168.100.72
10. Mitjançant el navegador apunta a aquesta IP. Accedeix amb l’ususari **admin** i  el password **Melda2018** 
11. Ara ja pots acabar de configurar la nodeMCU mitjançant l’interfície web.

Recordeu que l'unic fitxer que es pot modificar (ojo) és el: `C:\kk\espurna-1.13.0\code\espurna\config\hardware.h`

Exactament aquest tros:

```
#elif defined(NODEMCU_LOLIN)

    // Info
    #define MANUFACTURER        "NODEMCU"
    #define DEVICE              "LOLIN"

    // Buttons
    //
    #define BUTTON1_PIN           5
    #define BUTTON1_MODE          BUTTON_PUSHBUTTON //| BUTTON_DEFAULT_HIGH
    #define BUTTON1_PRESS         BUTTON_MODE_NONE
    #define BUTTON1_CLICK         BUTTON_MODE_TOGGLE//BUTTON_MODE_ON//BUTTON_MODE_OFF 
    #define BUTTON1_DBLCLICK	    BUTTON_MODE_NONE  //BUTTON_MODE_AP
    #define BUTTON1_LNGCLICK	    BUTTON_MODE_NONE  //BUTTON_MODE_RESET
    #define BUTTON1_LNGLNGCLICK   BUTTON_MODE_NONE  //BUTTON_MODE_FACTORY
    #define BUTTON1_RELAY         1
    
    #define BUTTON2_PIN           4
    #define BUTTON2_MODE          BUTTON_PUSHBUTTON //| BUTTON_DEFAULT_HIGH
    #define BUTTON2_RELAY         2

    #define BUTTON3_PIN           14
    #define BUTTON3_MODE          BUTTON_PUSHBUTTON //| BUTTON_DEFAULT_HIGH

    #define BUTTON4_PIN           12
    #define BUTTON4_MODE          BUTTON_PUSHBUTTON //| BUTTON_DEFAULT_HIGH
    
     //PIR
    //#define BUTTON5_PIN         10
    //#define BUTTON5_MODE        BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH | BUTTON_SET_PULLUP
    //#define BUTTON5_CLICK       BUTTON_MODE_TOGGLE
    //#define BUTTON5_DBLCLICK	  BUTTON_MODE_NONE  //BUTTON_MODE_AP
    //#define BUTTON5_LNGCLICK	  BUTTON_MODE_NONE  //BUTTON_MODE_RESET
    //#define BUTTON5_LNGLNGCLICK BUTTON_MODE_NONE  //BUTTON_MODE_FACTORY
    //#define BUTTON5_RELAY       4
    

    // Relays
    #define RELAY1_PIN            15
    #define RELAY1_TYPE           RELAY_TYPE_NORMAL
    #define RELAY2_PIN            13
    #define RELAY2_TYPE           RELAY_TYPE_NORMAL
    #define RELAY3_PIN            2
    #define RELAY3_TYPE           RELAY_TYPE_INVERSE
    #define RELAY4_PIN            0
    #define RELAY4_TYPE           RELAY_TYPE_INVERSE

    // LEDs
    #define LED1_PIN              16
    #define LED1_PIN_INVERSE      1
    
    // DB18B20//if (gpio == 10) return true;//Jaume Nogués al gpio.ino
    #define DALLAS_SUPPORT             	0
    #define DALLAS_PIN                 	10
    #define DALLAS_UPDATE_INTERVAL     	5000
    #define TEMPERATURE_MIN_CHANGE      1.0
    
    //DHT-11/22
    #define DHT_SUPPORT                 0
    #define DHT_PIN                     10
    #define DHT_TYPE                    DHT_CHIP_DHT11 //DHT_CHIP_DHT22

    // BME280/BMP280
    #define BMX280_SUPPORT              0
    #define I2C_SDA_PIN			            12//SDA 
    #define I2C_SCL_PIN			            14//SCL
    #define BMX280_ADDRESS              0x76    // 0x00 means auto
    #define BMX280_MODE                 1       // 0 for sleep mode, 1 or 2 for forced mode, 3 for normal mode
    #define BMX280_STANDBY              0       // 0 for 0.5ms, 1 for 62.5ms, 2 for 125ms
                                                    // 3 for 250ms, 4 for 500ms, 5 for 1000ms
                                                    // 6 for 10ms, 7 for 20ms
    #define BMX280_FILTER               0       // 0 for OFF, 1 for 2 values, 2 for 4 values, 3 for 8 values and 4 for 16 values
    #define BMX280_TEMPERATURE          1       // Oversampling for temperature (set to 0 to disable magnitude)
    #define BMX280_HUMIDITY             1       // Oversampling for humidity (set to 0 to disable magnitude, only for BME280)
    #define BMX280_PRESSURE             1       // Oversampling for pressure (set to 0 to disable magnitude)

    //Analog input
    #define ANALOG_SUPPORT      1
    #define ADC_VCC_ENABLED     1

```

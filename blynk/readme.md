# Com instal·lar el servidor de blynk en Windows

1. Instal·la [Java 10](https://www.oracle.com/technetwork/java/javase/downloads/jdk10-downloads-4416644.html).
2. Descarrega el servidor de blynk [server-0.38.4.jar](https://github.com/blynkkk/blynk-server/releases/download/v0.38.4/server-0.38.4.jar) a la carpeta `C:>blynk`.
3. Crea dins de `C:>blynk` un fitxer de text anomenat `mail.properties` amb aquest contingut:
```
    mail.smtp.auth=true
    mail.smtp.starttls.enable=true
    mail.smtp.host=smtp.gmail.com
    mail.smtp.port=587
    mail.smtp.username=fulanito@gmail.com
    mail.smtp.password=elMeuPassword
```

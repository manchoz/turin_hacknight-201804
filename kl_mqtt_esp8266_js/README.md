# Installing Espruino on Wemos D1

[Espruino](http://www.espruino.com) let's you run JavaScript on your favourite IoT MCU board.

Let's install Espruino on cheap ESP8266-based Wemos D1 Mini.

1. Download latest firmware for ESP8266 from [Espruino](http://www.espruino.com).

       curl -fsSLO https://www.espruino.com/binaries/espruino_1v99_esp8266_4mb_combined_4096.bin

2. Install `esptool` from `pip` or from your distro's packages repository.

3. Plug the Wemos D1 to a free USB port and check for proper ESP8266 connectivity.

       esptool.py chip_id

4. Flash firmware

       esptool.py write_flash 0x0 espruino_1v99_esp8266_4mb_combined_4096.bin

5. Check installation connecting to Espruino JavaScript REPL via serial terminal emulator (eg. screen, miniterm.py, picocom or minicom).

6. Install [Espruino Web IDE](http://www.espruino.com/Web+IDE).

7. Enjoy JavaScript for IoT!

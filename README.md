# stm32-iap-demo
for iap
burn command:
openocd -f /usr/share/openocd/scripts/interface/stlink-v2.cfg -f /usr/share/openocd/scripts/target/stm32f1x.cfg -c init -c "reset halt; wait_halt; flash write_image erase ./build/stm32f103_iap.bin 0x08000000; reset; run; shutdown"

for app
burn command:
openocd -f /usr/share/openocd/scripts/interface/stlink-v2.cfg -f /usr/share/openocd/scripts/target/stm32f1x.cfg -c "init; reset halt; wait_halt; flash write_image erase ./build/f103_app.bin 0x08002000; reset; shutdown"


Use minicom to send file(CTRL A + S and select ascii)



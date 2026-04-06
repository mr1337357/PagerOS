PREFIX=/home/misspapaya/.arduino15/packages/esp32/tools/esp-x32/2511/bin/xtensa-esp32s3-elf
ADDR2LINE=${PREFIX}-addr2line
CC=${PREFIX}-gcc
STRIP=${PREFIX}-strip
ELFEDIT=${PREFIX}-elfedit
OBJDUMP=${PREFIX}-objdump

${ADDR2LINE} -f -p -e build/esp32.esp32.tlora_pager/PagerOS.ino.elf -a $1
#${OBJDUMP} -S build/esp32.esp32.tlora_pager/PagerOS.ino.elf > PagerOS.ino.asm

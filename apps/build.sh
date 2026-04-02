PREFIX=/home/misspapaya/.arduino15/packages/esp32/tools/esp-x32/2511/bin/xtensa-esp32s3-elf
CC=${PREFIX}-gcc
STRIP=${PREFIX}-strip
ELFEDIT=${PREFIX}-elfedit
OBJDUMP=${PREFIX}-objdump

CFLAGS="-fPIE -nostdlib"
# -static"
#CFLAGS="-static-pie -nostdlib"

${CC} ${CFLAGS} lib.c test.c -o test.elf
${CC} ${CFLAGS} lib.c hello.c -o hello.elf

#ls ${PREFIX}*
${STRIP} test.elf -o test.stripped.elf
${STRIP} hello.elf -o hello.stripped.elf

${PREFIX}-readelf -a test.elf > test.elf.readelf
${PREFIX}-readelf -a hello.elf > hello.elf.readelf
${PREFIX}-readelf -a hello.stripped.elf > hello.stripped.elf.readelf

${PREFIX}-objdump -S test.elf > test.elf.dump
${PREFIX}-objdump -S hello.elf > hello.elf.dump

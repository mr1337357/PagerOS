PREFIX=/home/misspapaya/.arduino15/packages/esp32/tools/esp-x32/2511/bin/xtensa-esp32s3-elf
CC=${PREFIX}-gcc
STRIP=${PREFIX}-strip
ELFEDIT=${PREFIX}-elfedit
OBJDUMP=${PREFIX}-objdump

CFLAGS="-fPIC -nostdlib -static"

${CC} ${CFLAGS} test.c lib.c -o test.elf
${CC} ${CFLAGS} hello.c lib.c -o hello.elf

#ls ${PREFIX}*
${STRIP} test.elf -o test.stripped.elf
${STRIP} hello.elf -o hello.stripped.elf

${PREFIX}-readelf -a test.elf > test.elf.readelf
${PREFIX}-readelf -a hello.elf > hello.elf.readelf

${PREFIX}-objdump -S test.elf > test.elf.dump
${PREFIX}-objdump -S hello.elf > hello.elf.dump

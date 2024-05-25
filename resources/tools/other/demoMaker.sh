#!/bin/bash
pio pkg install -e min
rm -rf resources/personal/*
cp -r resources/demo/* resources/personal/

cd resources/tools/
export PLATFORMIO_ENV_NAME="min"
./generate.sh
cd ../../

old_string="#define VAULT_PASSWORD \"\""
new_string="#define VAULT_PASSWORD \"1\""
filename="src/defines/confidential.h"

sed -i "s/$old_string/$new_string/g" "$filename"

old_string="#define VAULT 0"
new_string="#define VAULT 1"
filename="src/defines/config.h"

sed -i "s/$old_string/$new_string/g" "$filename"

old_string="#define BOOK 0"
new_string="#define BOOK 1"
filename="src/defines/config.h"

sed -i "s/$old_string/$new_string/g" "$filename"

# To apply those things
cd resources/tools/
export PLATFORMIO_ENV_NAME="min"
./generate.sh
cd ../../

pio run -e min

cd resources/tools/fs/
python3 generatePartTable.py
cd ../../../

pio run -e min
pio run -e min -t upload

sleep 10

cd resources/tools/fs/
./flashFs.sh
cd ../../../

sleep 10

cd resources/tools/other/
mkdir out/
esptool.py read_flash 0x00000 0x400000 out/demo.bin
sleep 5
esptool.py erase_flash
sleep 5
esptool.py write_flash 0x00000 out/demo.bin
cd ../../../
zip -r -9 resources/tools/other/out/demo-source.zip .
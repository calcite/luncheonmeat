# luncheonmeat

## Hardware

Device PCB was designed using KiCAD 5.1. For more info visit KiCAD website [kicad.org](https://kicad.org/download/) 

## Firmware

### Dependencies

```shell
sudo apt-get install gcc-avr binutils-avr avr-libc gdb-avr avrdude
```

> Note: ``ATMega328PB`` is new revision from 2017. avr-libc may not have support for this device. To add support please see how its done in .devcontainer/Dockerfile.

### Build

```shell
cd firmware
make
```

### Flashing

Flashing the device using AVR Dragon ISP:

```shell
make program
```

Set fuses **this is done only once**:

```shell
make program-fuses
```

> Note: ``make`` will internally run ``avrdude`` with correct atributes. 

Bare avrdude flashing command:

```shell
avrdude -p atmega328pb -P usb -c dragon_isp    -U flash:w:luncheonmeat.hex -U eeprom:w:luncheonmeat.eep
```


### Serial Bootloader

Generating ``defines.h`` file:
```shell
cd firmware/bootloader
./preprocessor.sh atmega328pb 2048 PORTB PINB0 8000000 19200
make
make program
make program-fuses
```

Reading target fuses:
```shell
avrdude -p atmega328pb -P usb -c dragon_isp -U efuse:r:-:h
```

To enter bootloader hold ''5'' button and reset the device.

## Resources

- [fuses calculator](http://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega328p&LOW=E2&HIGH=D8&EXTENDED=F7&LOCKBIT=FF)

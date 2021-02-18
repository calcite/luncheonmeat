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


### Serial Bootloader (not implemented)

Generating ``defines.h`` file:
```shell
./preprocessor.sh atmega328pb 2048 PORTB PINB7 8000000 19200
```

## Resources

- [fuses calculator](http://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega88p&LOW=4C&HIGH=DF&EXTENDED=F9&LOCKBIT=FF)

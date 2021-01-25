# luncheonmeat

## Hardware

TBD

## Firmware

Dependencies:

```shell
sudo apt-get install gcc-avr binutils-avr avr-libc gdb-avr avrdude
```

Build:

```shell
cd firmware
make
```

Flash:

```shell
make flash
```

### bootloader

Generating ``defines.h`` file:
```shell
./preprocessor.sh atmega328pb 2048 PORTB PINB7 8000000 19200
```

## Resources

- [fuses](http://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega8&LOW=4C&HIGH=DF&LOCKBIT=FF)


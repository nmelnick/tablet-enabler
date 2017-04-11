TabletEnabler
=============

A Mac OS X kernel extension, usable in 32 or 64 bit, to activate the serial
tablet device on the ThinkPad X61 tablet. The BIOS on that machine does not
activate the device automatically, so this forces OS X to allocate memory and
create a serial port for it. It will load early in the boot process so that the
Apple serial driver can then find the device and allocate a serial device node
to it.

This was last tested and compiled on Mac OS X 10.6 Snow Leopard. No guarantees
are provided that this works on versions after 10.6, or, frankly, on 10.6.

This code is pulled from the depths of the mid-late-2000s, and consists of a ton
of frustration, a long series of hacks, and a fundamental lack of knowledge of
OS X kernel development and IOKit. I wanted to post it up somewhere before it
finally left the depths of my 'Old Projects' directory.

This project likely requires XCode 3.x, and may/should compile with XCode 4.x
on 10.6.8. Compiling will result in a working kext that can be placed in
/Library/Extensions. For the serial port to activate, the Apple serial kext
must be edited. Change the file:

```
/System/Library/Apple16X50Serial.kext/Contents/PlugIns/Apple16X50ACPI.kext/Contents/Info.plist
```

and replace the instance of `PNP0501` with `WACF004` or `WACF008`, depending on
your device.

The tablet itself will require a driver for OS X to care about it, and this was
tested with [TabletMagic](http://www.thinkyhead.com/tabletmagic).

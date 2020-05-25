# CP210xRelay

This project contain windows command line software and cad drawings for a relay box based on a CP210x USB chip. Originally used for test automation. 

![image of assembled relay box](/assembled_box.png)

# Part list
- [Conrad Components 393905 Relay card Component 5 V DC](https://www.conrad.-com/p/conrad-components-393905-relay-card-component-5-v-dc-393905)
- [Low power extension cable](https://www.conrad.com/p/tru-components-low-power-extension-cable-low-power-plug-low-power-socket-55-mm-21-mm-55-mm-21-mm-1-pcs-1582277)
- [USB 2.0 Cable](https://www.conrad.com/p/goobay-usb-20-cable-1x-usb-20-connector-a-1x-usb-20-connector-b-180-m-grey-973569)
- [3D-printed enclosure](#box)

# Software
This tool is for windows. Linux has a [kernel driver](https://github.com/torvalds/linux/blob/master/drivers/usb/serial/cp210x.c) for the chip so there are some options.

## Build
Build using CMake or open with Visual Studio 2019 which support CMake out of the box.

## Preparation 
A CP210x from the factory require *either*:
- Installation of a [windows driver](https://www.silabs.com/documents/public/software/CP210x_Universal_Windows_Driver.zip)
- or to reprogram the chip to utilize the driver distributed wih windows.

### Reprogramming procedure
To make the box plug-and-play with windows without driver installation, follow this [guide](https://www.silabs.com/documents/public/application-notes/AN721.pdf) (or perhaps quicker using [CP210xSetIDs.exe](https://www.silabs.com/content/usergenerated/asi/cloud/attachments/siliconlabs/en/community/groups/interface/knowledge-base/jcr:content/content/primary/blog/cp210x_legacy_progra-zARf/CP210x_LegacyUtilities.zip) from [legacy utilitities](https://www.silabs.com/community/interface/knowledge-base.entry.html/2016/11/04/cp210x_legacy_progra-zARf)) and reprogram the chip´s VID to *0x10C4* and PID to *0xEA63*.

# How to run
## Examples:

- Turn on first relay: `CP210xRelay.exe 0 1`
- Turn off first relay: `CP210xRelay.exe 0 0`
- Turn on second relay: `CP210xRelay.exe 1 1`
- Select one of many connected boxes: `CP210xRelay.exe --port COM4 0 1`

## Help: 
```
$  ./CP210xRelay.exe --help
Usage: CP210xRelay.exe [OPTION]... RELAY_NUMBER NEW_STATE
Control the state of a CP210x GPIO based relay board.

RELAY_NUMBER  Which relay to control. (0 to 3)
NEW_STATE     1 turns on the relay, 0 turns it off.

All COM ports are probed by default to find the relay board. If
exactly one relay board is found, it is used without the need to
specify it explicitly.

 -p, --port   COM port for the relay board to control
              Example: "COM4"
 --test       Cycle all relays on and off
```

# Box
The relay card enclosure can be 3D-printed using the pre-exported [amf file](/cad/relay_card_box.amf) or [opened](/cad/relay_card_box.FCStd) and modified using [FreeCAD](https://www.freecadweb.org/).

![relay box](/cad/relay_card_box.png)

## Assembly
Put relay card into enclosure. Put some force where the mounting holes are and press it down and it will stay in place.


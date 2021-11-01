This program reads keyboard events from a Linux /dev/input/eventX device, and injects most of the events back into the kernel using /dev/uinput. Currently, the only special thing that happens is when using the caps lock key. If you just press and release it, an 'escape' key will be emulated instead. If you hold the caps lock and press another key, the caps lock actually emulates a (left) control key.

To compile it, something like this should work:

	# First download the zip file, brmtron-keymod-(revision number).zip
	unzip brmtron-keymod-(revision number).zip
	cd brmtron-keymod-(revision number)
	mkdir build
	cd build
	cmake .. && make

The program should be run as root, so using sudo may be helpful. It takes two arguments, the event device to read events from and the uinput device. For example:

	sudo ./keymod /dev/input/event2 /dev/uinput

To find out which eventX you need to use, take a look at the /proc/bus/input/devices file. For example, in my case there's an entry like this:

	I: Bus=0011 Vendor=0001 Product=0001 Version=ab41
	N: Name="AT Translated Set 2 keyboard"
	P: Phys=isa0060/serio0/input0
	S: Sysfs=/devices/platform/i8042/serio0/input/input2
	U: Uniq=
	H: Handlers=sysrq kbd event2 
	B: PROP=0
	B: EV=120013
	B: KEY=402000000 3803078f800d001 feffffdfffefffff fffffffffffffffe
	B: MSC=10
	B: LED=7

From the "H:" line this shows that I'd need to use /dev/input/event2. Included is also a script `geteventdevice.py` which obtains the event file based on a string that appears in the "N:" field. So in this case

	./geteventdevice.py "AT Translated Set"

would output

	/dev/input/event2

This may be useful if the precise eventX number tends to change. So in that case, the following could be used:

	sudo ./keymod `./geteventdevice.py "AT Translated Set"` /dev/uinput


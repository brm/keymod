#include "uinputdevice.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

UInputDevice::UInputDevice()
{
	m_dev = -1;
}

UInputDevice::~UInputDevice()
{
	close();
}

bool UInputDevice::open(const std::string &fileName)
{
	if (m_dev >= 0)
	{
		setErrorString("Device already open");
		return false;
	}

	// let's create an uinput device
	
	int fkbd;
	struct input_event inevt;
	struct uinput_user_dev device;
	unsigned short k;

	// create keyboard device
	
	fkbd = ::open(fileName.c_str(), O_RDWR);
	if (fkbd < 0)
	{
		setErrorString("Can't open uinput device " + fileName);
		return false;
	}

	memset(&device, 0, sizeof(device));
	strcpy(device.name, "Input-modifier");
	device.id.bustype = BUS_USB;
	device.id.vendor = 1;
	device.id.product = 1;
	device.id.version = 1;
	
	if (write(fkbd, &device, sizeof(struct uinput_user_dev)) < 0)
	{
		setErrorString("Can't register new device");
		::close(fkbd);
		return false;
	}

	ioctl(fkbd, UI_SET_EVBIT, EV_KEY);
	for (k = 1; k < KEY_MAX ; k++)
		ioctl(fkbd, UI_SET_KEYBIT, k);

	if (ioctl(fkbd,UI_DEV_CREATE) < 0)
	{
		setErrorString("Can't create the new fake device");
		::close(fkbd);
		return false;
	}

	m_dev = fkbd;

	return true;
}

bool UInputDevice::close()
{
	if (m_dev < 0)
	{
		setErrorString("No device open");
		return false;
	}
	ioctl(m_dev, UI_DEV_DESTROY);
	::close(m_dev);

	return true;
}

bool UInputDevice::writeKey(int keyCode, bool pressed)
{
	struct input_event event;

	memset(&event, 0, sizeof(event));
	event.type = EV_KEY;
	event.value = (pressed)?1:0;
	event.code = keyCode;
	if (write(m_dev, &event, sizeof(event)) != sizeof(event))
	{
		setErrorString("Error writing key event");
		return false;
	}

	memset(&event, 0, sizeof(event));
	event.type = EV_SYN;
	if (write(m_dev, &event, sizeof(event)) != sizeof(event))
	{
		setErrorString("Error writing SYN event");
		return false;
	}

	return true;
}

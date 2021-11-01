/*

  This file is a part of xevdevserver, a program that reads events using 
  the evdev input event interface of the Linux kernel and sends them to an
  X display using the XTEST extension.
  
  Copyright (C) 2006-2007  Jori Liesenborgs (jori.liesenborgs@gmail.com)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#include "eventdevice.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/input.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <unistd.h>

EventDevice::EventDevice(const std::string &name) : errut::ErrorBase(name)
{
	m_fileDescriptor = -1;
}

EventDevice::~EventDevice()
{
	close();
}

bool EventDevice::open(const std::string &deviceName, bool exclusive)
{
	if (m_fileDescriptor != -1)
	{
		setErrorString("A device has already been opened");
		return false;
	}
	
	// We'll open in non-blocking mode so a 'read' call will definitely never block the
	// application

	int fd = ::open(deviceName.c_str(), O_RDWR|O_NONBLOCK);

	if (fd < 0)
	{
		setErrorString("Unable to open device");
		return false;
	}

	if (exclusive)
	{
		int val = 1;
		
		if (ioctl(fd, EVIOCGRAB, &val) < 0)
		{
			setErrorString("Unable to get exclusive access to the device");
			::close(fd);
			return false;
		}
	}

	m_fileDescriptor = fd;

	return true;
}

bool EventDevice::close()
{
	if (m_fileDescriptor == -1)
	{
		setErrorString("No device has been opened yet");
		return false;
	}

	::close(m_fileDescriptor);
	return true;
}

bool EventDevice::setExclusive(bool exclusive)
{
	int val = 1;
	int *pVal = &val;

	if (!exclusive)
		pVal = NULL;

	if (ioctl(m_fileDescriptor, EVIOCGRAB, pVal) < 0)
	{
		setErrorString("Unable to adjust exclusive access");
		return false;
	}

	return true;
}

bool EventDevice::processEvents()
{
	struct input_event event;
	ssize_t status = 0;

	while ((status = read(m_fileDescriptor, &event, sizeof(struct input_event))) == sizeof(struct input_event))
		processEvent(&event);

	if (status == -1)
	{
		if (!(errno == EAGAIN || errno == EINTR))
		{
			std::string errorMessage(strerror(errno));
			
			setErrorString(std::string("Unable to read from event device: ") + errorMessage);
			return false;
		}
	}
	
	return true;
}

void EventDevice::writeEvent(uint16_t eventType, uint16_t eventCode, int32_t eventValue)
{
	struct input_event event;

	event.type = eventType;
	event.code = eventCode;
	event.value = eventValue;

	write(m_fileDescriptor, &event, sizeof(struct input_event));
}
void EventDevice::onIgnoredEvent(const struct input_event *pEvent)
{
	std::cerr << "[" << getObjectName() << "] Ignoring event with type = " << pEvent->type << ", code = " << pEvent->code << ", value = " << pEvent->value << std::endl;
}


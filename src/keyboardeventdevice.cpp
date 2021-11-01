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

#include "keyboardeventdevice.h"
#include <linux/input.h>

KeyboardEventDevice::KeyboardEventDevice(const std::string &name) : EventDevice(name)
{
	m_useRepeatMessages = false;
}

KeyboardEventDevice::~KeyboardEventDevice()
{
}

void KeyboardEventDevice::processEvent(const struct input_event *pEvent)
{
	struct input_event event = *pEvent;
	
	switch(event.type)
	{
	case EV_KEY:
		if (event.value != 2) // Not an auto repeat
		{
			bool pressed = (event.value == 1)?true:false;

			onKeyPress(event.code, pressed, false);
		}
		else // Auto repeat message
		{
			if (m_useRepeatMessages)
			{
				onKeyPress(event.code, false, true);
				onKeyPress(event.code, true, true);
			}
			else
				onIgnoredEvent(pEvent);
		}
		break;
	default:
		onIgnoredEvent(pEvent);
	}
}

void KeyboardEventDevice::setNumLock(bool f)
{
	int32_t value = (f)?1:0;

	writeEvent(EV_LED, LED_NUML, value);
}

void KeyboardEventDevice::setCapsLock(bool f)
{
	int32_t value = (f)?1:0;

	writeEvent(EV_LED, LED_CAPSL, value);
}

void KeyboardEventDevice::setScrollLock(bool f)
{
	int32_t value = (f)?1:0;

	writeEvent(EV_LED, LED_SCROLLL, value);
}

void KeyboardEventDevice::setUseRepeatMessages(bool f)
{
	m_useRepeatMessages = f;
}


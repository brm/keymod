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

#ifndef KEYBOARDEVENTDEVICE_H

#define KEYBOARDEVENTDEVICE_H

#include "eventdevice.h"

class KeyboardEventDevice : public EventDevice
{
public:
	KeyboardEventDevice(const std::string &name);
	~KeyboardEventDevice();

	void setNumLock(bool f);
	void setCapsLock(bool f);
	void setScrollLock(bool f);
	void setUseRepeatMessages(bool f);
protected:
	void processEvent(const struct input_event *pEvent);

	virtual void onKeyPress(int eventCode, bool pressed, bool isrepeat)				{ }
private:
	bool m_useRepeatMessages;
};

#endif // KEYBOARDEVENTDEVICE_H


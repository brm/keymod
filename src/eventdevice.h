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

#ifndef EVENTDEVICE_H

#define EVENTDEVICE_H

#include "errorbase.h"
#include <stdint.h>

struct input_event;

class EventDevice : public errut::ErrorBase
{
public:
	EventDevice(const std::string &name);
	~EventDevice();

	bool open(const std::string &deviceName, bool exclusive);
	bool close();

	bool setExclusive(bool exclusive);

	int getFileDescriptor() const						{ return m_fileDescriptor; }

	bool processEvents();
	void writeEvent(uint16_t eventType, uint16_t eventCode, int32_t eventValue);
protected:
	virtual void processEvent(const struct input_event *pEvent)		{ onIgnoredEvent(pEvent); }
	virtual void onIgnoredEvent(const struct input_event *pEvent);
private:
	int m_fileDescriptor;
};
#endif // EVENTDEVICE_H

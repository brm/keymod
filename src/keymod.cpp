/*
  Based on xevdevkeyinfo.cpp from xevdevserver

  -----

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
#include "uinputdevice.h"
#include <time.h>
#include <linux/input.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <iostream>
#include <vector>

using namespace std;

void printUsage()
{
	cout << endl;
	cout << "Usage:" << endl << "keymod keyboarddevice uinputdevice" << endl;
	cout << endl;
	exit(-1);
}

class DummyKeyboardEventDevice : public KeyboardEventDevice
{
public:
	DummyKeyboardEventDevice(UInputDevice *pUInputDev) : KeyboardEventDevice("DummyKeyboardEventDevice")
	{
		m_pUInputDev = pUInputDev;
		m_gotOtherKey = false;
		m_capsPressed = false;

		for (int i = 0 ; i < KEY_MAX ; i++)
			m_pressedKey[i] = false;
	}

	~DummyKeyboardEventDevice()
	{
	}

	void releasePressedKeys()
	{
		for (int i = 0 ; i < KEY_MAX ; i++)
		{
			if (m_pressedKey[i])
			{
				m_pUInputDev->writeKey(i, false);
				m_pressedKey[i] = false;
			}
		}

		m_capsPressed = false;
		m_gotOtherKey = false;
	}
protected:
	void onKeyPress(int eventCode, bool pressed, bool isrepeat)
	{
		if (eventCode == KEY_CAPSLOCK)
		{
			if (!isrepeat)
			{
				if (!pressed)
				{
					// If no other key got pressed, send ESC
					if (!m_gotOtherKey)
					{
						m_pUInputDev->writeKey(KEY_ESC, true);
						m_pUInputDev->writeKey(KEY_ESC, false);

						m_pressedKey[KEY_ESC] = false;
					}
					else
					{
						// Need to release CTRL
						m_pUInputDev->writeKey(KEY_LEFTCTRL, false);

						m_pressedKey[KEY_ESC] = false;
					}
				}
				else
					m_gotOtherKey = false;

				m_capsPressed = pressed;
			}
		}
		else
		{
			if (pressed)
			{
				if (m_capsPressed)
				{
					if (!m_gotOtherKey)
					{
						// First emulate the press of Ctrl
						m_pUInputDev->writeKey(KEY_LEFTCTRL, true);

						m_pressedKey[KEY_LEFTCTRL] = true;
					}

					m_gotOtherKey = true;
				}
			}

			m_pUInputDev->writeKey(eventCode, pressed);

			if (eventCode < KEY_MAX)
				m_pressedKey[eventCode] = pressed;
		}
	}

	void onIgnoredEvent(const struct input_event *pEvent)
	{
	}
private:
	UInputDevice *m_pUInputDev;
	
	bool m_capsPressed;
	bool m_gotOtherKey;

	bool m_pressedKey[KEY_MAX];
};

int processDeviceEvents(DummyKeyboardEventDevice &kbdDevice)
{
	int selectErrors = 0;

	while (true) // TODO: what is a good criterion here?
	{
		fd_set fdSet;

		FD_ZERO(&fdSet);
		FD_SET(kbdDevice.getFileDescriptor(), &fdSet);
		FD_SET(0, &fdSet);

		if (select(FD_SETSIZE, &fdSet, 0, 0, 0) == -1)
		{
			cerr << "Error in select" << endl;
			selectErrors++;

			if (selectErrors > 4)
				return -1;
		}
		else
			selectErrors = 0;

		if (FD_ISSET(kbdDevice.getFileDescriptor(), &fdSet))
		{
			if (!kbdDevice.processEvents())
			{
				cerr << kbdDevice.getErrorString() << endl;
				return -1;
			}
		}
	}
	
	return 0;
}

DummyKeyboardEventDevice *pKbdDev = 0;

void terminalStopHandler(int val)
{
	cerr << endl << "Got SIGTSTP, releasing keyboard device" << endl << endl;
	pKbdDev->releasePressedKeys();
	pKbdDev->setExclusive(false);

	// Send STOP to outselves
	cerr << "Sending SIGSTOP to ourselves" << endl;
	kill(getpid(), SIGSTOP);
}

void continueHandler(int val)
{
	cerr << endl << "Got SIGCONT, trying to capture event device exclusively again" << endl << endl;
	if (!pKbdDev->setExclusive(true))
	{
		cerr << "setExclusive failed: " << pKbdDev->getErrorString() << endl;
		exit(-1);
	}
}

int main(int argc, char *argv[])
{
	if (argc != 3)
		printUsage();

	string kbdName = string(argv[1]);
	string uinputName = string(argv[2]);
	
	UInputDevice uinputDev;

	if (!uinputDev.open(uinputName))
	{
		cerr << "Error opening uinput device: " + uinputDev.getErrorString() << endl;
		return -1;
	}
	
	DummyKeyboardEventDevice kbdDevice(&uinputDev);

	if (!kbdDevice.open(kbdName, true))
	{
		cerr << "Couldn't open " << kbdName << ": " << kbdDevice.getErrorString() << endl;
		return -1;
	}

	kbdDevice.setUseRepeatMessages(true);

	// Install signal handlers

	pKbdDev = &kbdDevice;

	signal(SIGTSTP, terminalStopHandler);
	signal(SIGCONT, continueHandler);

	int status = processDeviceEvents(kbdDevice);

	return status;
}


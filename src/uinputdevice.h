#ifndef UINPUTDEVICE_H

#define UINPUTDEVICE_H

#include "errorbase.h"

class UInputDevice : public errut::ErrorBase
{
public:
	UInputDevice();
	~UInputDevice();
	bool open(const std::string &fileName);
	bool close();

	bool writeKey(int keyCode, bool pressed);
private:
	int m_dev;
};

#endif // UINPUTDEVICE_H

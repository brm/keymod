cmake_minimum_required(VERSION 2.8)

project(keymod)
set(VERSION 0.0.1)

add_executable(keymod src/keymod.cpp src/eventdevice.cpp src/keyboardeventdevice.cpp src/uinputdevice.cpp)


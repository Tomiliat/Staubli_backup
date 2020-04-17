ROS 3d printer project. 
02.02.2020-31.5.2020

This package is for the printer part of the project. 3D Printer is built in common way, using Arduino MEGA 2560, Ramps 1.4 card and inside it is running Marlin software.

Shortly: printertool.cpp is sending G-code to the usb port, one line at the time. MySerial packages handle connection to the port.

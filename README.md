# TestArduino

###A C++ application to interface with an Arduino.

TargetPlatform: Windows. (Highly OS-specific COM Port API)

Design in progress on low-latency Arduino-PC communication software.

##Purpose

The ultimate intended purpose is scientific data acquisition (at a 2% the cost, with comparable effort).

The Arduino connects to the PC via USB cable.  The design has been tested successfully, so far, with an Arduino Uno (R3), and Arduino Due.

The original usecase is an interactive psychology experiment (explaining some of the quirks of the naming conventions in the Arduino sketch).

Matlab-Arduino interface (via COM port, not the available MatLab sketch) is also in development.

##Install

The C++ software was developed on a Windows 8 machine with Visual Studio 2013.  Outside of this platform, your mileage may vary.  No special API needed, as far as I am aware, outside of the default installs.  Probably WinAPI or something of the sort.

The Arduino sketches can be flashed to any Arduino-compatible device with the Arduino IDE.

Any MatLab scripts can be run with MatLab.  Modifications would be required to be compatible with Octave.  MatLab scripts developed on R2014a, but it should not be specifically required.

##Latency Details

When paired with the collection of Neural data, it is important to get latency as low and consistent as possible.  Unfortunately, in this respect, MatLab fairs poorly.  It appears that for comparable tasks, the MatLab Serial functionality (derived from Java) is noticeably more variable.

###A simple Serial Reflection task for profiling:
  Send a character, have the Arduino send that character back, but embed this logic in the middle of its experiment monitoring routines to create a realistic profile of response time.  Repeat 10,000 times.

For this task, using an Arduino Due, the C++ program had a response with these characteristics:
* Average Response Time:                4.096 ms
* Standard Deviation of Response Time:  0.016 ms
* Max Response Time:                    4.181 ms

The same hardware and software communicating with MatLab, however saw these characteristics:
* Average Response Time:                4.000 ms  !!UPDATE!!
* Standard Deviation of Response Time:  3.000 ms  !!UPDATE!!
* Max Response Time:                   22.000 ms  !!UPDATE!!

The likely cuplrit in MatLab's case is likey Garbage Collection, and there's not a lot we can do about that.  In any case, since Matlab is required for the task, the best solution may be to open a connection between MatLab and the C++ application, and let C++ handle the Computer<->Arduino communication.


##Known Issues

* Sometimes the program will either fail to find or fail to connect to the Arduino.  In many cases, this seems to be because some remnants of old messages trickle in.  Peculiar.  Working on increasing the consistency of operation, but in general it seems pretty good.  Play with timing to help address this.

##To Do

* Create a Unix platform option.  Current Windows dependenices exist only in Serial.h, Serial.cpp, and CoreFunctions.cpp.

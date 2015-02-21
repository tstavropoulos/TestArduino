# TestArduino

###A C++ application to interface with an Arduino.

TargetPlatform: Windows. (Highly OS-specific COM Port API)

Design in progress on low-latency Arduino-PC communication software.

Communication is available in three flavors:
* MatLab's Java-Based Serial package
* Native C++/WinAPI serial communicaiton
* C++/WinAPI DLL to import into MatLab

##Purpose

The ultimate intended purpose is scientific data acquisition (at a 2% the cost, with comparable effort).

The Arduino connects to the PC via USB cable.  The design has been tested successfully, so far, with an Arduino Uno (R3), and Arduino Due.

The original usecase is an interactive psychology experiment (explaining some of the quirks of the naming conventions in the Arduino sketch).

##Install

The C++ software was developed on a Windows 8.1 machine with Visual Studio 2013.  Outside of this platform, your mileage may vary.  No special API needed, as far as I am aware, outside of the default installs.  Probably WinAPI or something of the sort.

The Arduino sketches can be flashed to any Arduino-compatible device with the Arduino IDE.

Any MatLab scripts can be run with MatLab.  Modifications would be required to be compatible with Octave.  MatLab scripts developed on R2014a, but it should not be specifically required.

##What do I need?

###Windows

Well, for starters you will probably need a recent release of Visual Studio.  If *free* is in your price range, and you're developing in a Windows environment, you might want to check out [Microsoft Visual Studio - Community 2013](http://www.visualstudio.com/downloads/).  If you're not developing in Windows, you better know what you're doing, because the libraries are currently heavily dependent on the Windows API.

I'm going to try to keep reasonably recent builds of the library files uploaded in the BuiltLibs folder, so it won't be strictly necessary to have the IDE installed.

You will also need any version of the Arduino IDE that works with your Arduino.  The Arduino IDE is available from [The Official Arduino Website](http://arduino.cc/en/main/software).  Speaking of which, you will probably need an Arduino.

If you wish to utilize the MatLab scripts, you'll need a version of MatLab that supports the loading of external libraries.  I doubt that is a new feature, any distro of MatLab released in the last 5 years is probably sufficient.  I am going to investigate getting the libraries to build and link with Octave (an opensource MatLab alternative), but that's on the To Do list for now.

For very low latency, I recommend getting:
* [An Arduino RS232 Shield](https://www.sparkfun.com/products/13029)
    * [I modified mine to auto-restart](https://imgur.com/a/rPBKA#0)
    * [Or you can build your own](http://arduino.cc/en/Tutorial/ArduinoSoftwareRS232)
* [A PCIe card with a RS232 Port](http://www.newegg.com/Product/Product.aspx?Item=N82E16815124084)

###Unix

If you are developing in Unix, you will need:

* g++ version 4.8+
* g++ multilib
* libc6-dev-i386
* Code::Blocks or familiarity with compiling source code
* ARM, x86, or x64 processor

There may be a better way, but you should create a link to the compiled librarys in your /usr/lib directory, so that you always use the latest version.

    sudo ln -sf libArduinoCommDLLx86.so /usr/lib/libArduinoCommDLLx86.so
    sudo ln -sf libArduinoCommDLLx64.so /usr/lib/libArduinoCommDLLx64.so
    sudo ln -sf libArduinoCommDLLARM.so /usr/lib/libArduinoCommDLLARM.so

##Latency Details

When paired with the collection of Neural data, it is important to get latency as low and consistent as possible.  Unfortunately, in this respect, MatLab fairs poorly.  It appears that for comparable tasks, the MatLab Serial functionality (derived from Java) is noticeably more variable.

###A simple Serial Reflection task for profiling:
  Send a character, have the Arduino send that character back, but embed this logic in the middle of its experiment monitoring routines to create a realistic profile of response time.  Repeat 10,000 times.

For this task, using an Arduino Due, the C++ program had a response with these characteristics:
* Average Response Time:                4.096 ms
* Standard Deviation of Response Time:  0.016 ms
* Max Response Time:                    4.181 ms

The same hardware and software communicating with MatLab, however saw these characteristics:
* Average Response Time:                9.497 ms
* Standard Deviation of Response Time:  1.852 ms
* Max Response Time:                   66.540 ms

Communicating with MatLab through the C++ Shared Library:
* Average Response Time:                4.093 ms
* Standard Deviation of Response Time:  0.295 ms
* Max Response Time:                    5.049 ms

Communicating with MatLab through the C++ Shared Library and a PCIe RS232 Card to the RS232 Arduino Shield:
* Average Response Time:                0.7139 ms
* Standard Deviation of Response Time:  0.0055 ms
* Max Response Time:                    0.7923 ms

##Known Issues


##To Do

* Test Unix port.
* Explore portability to Octave.  Will require changes to project build.
* Test response time with RS232 communication.

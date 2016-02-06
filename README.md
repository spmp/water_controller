Water Controller
================

The aim of this project is to control and monitor a water heating system.
This is currently a **Work in Progress**, and it is largely incomplete.

The project will consist of four main parts:

  1. An atmega328 MCU with a control system that directly interfaces with
     the water heater. It also operates over a serial interface to interact
     with a PC.

  2. A daemon running on the PC which can talk to the MCU. The daemon receives
     regular logging information from the MCU which it saves to a database.
     The daemon also allows various commands to be sent to the MCU.

  3. A web application that can display logs from the database, and can
     control the MCU via the daemon.
     
  4. A schematic and PCB artwork.

Currently only (1) is functional.

The _driveres_ for the project are in `AVR-lib`. At this stage I am (slowly) moving to github for issue tracking etc.

MCU
---

The project uses the Arduino Nano for both prototyping and target operation. To upload the code do the following:

 1. Plug in the MCU via USB to your computer. Check that `lsusb` shows the
    device, and that /dev/ttyUSB0 is writable (add your user to that device's
    group if not).

 2. Install the avr gcc toolchain. In Ubuntu/Debian, the packages you need
    (at least) are `avr-gcc`, `avr-libc`, and `avrdude` and their
    dependencies.

 3. Run `make` to compile the source, and `make upload` to compile and upload
    to the MCU via `/dev/ttyUSB*`. The program will start running immediately.

At present, the MCU reads data from the following inputs:

 * Pressure sensor (MPXV4006 I think...) via 12 bit I2C ADC (MPXV4006)
 * Temperature sensors (DS18B20) via OW to I2C (DS2482-100) (WIP)

These are sent over the serial interface to the PC along with a "timestamp" (the
number of seconds since the program started) at regular intervals.

To view the output, run `screen /dev/ttyUSB0 38400` (exit with `C-a k y`).

Logging daemon (logger.py)
--------------------------

This saves the data sent from the MCU to a sqlite3 database. Written in
Python3. To run it, just run

    $ ./logger.py

when the MCU is connected to the computer. The program will print out the
messages it recieves from the MCU, and also insert them into the database
`log_timeseries.db`. Type `Ctrl-C` or `Ctrl-D` to exit. You can view the
database directly using `sqlite3 log_timerseries.db`.

The logger should also have an interface for sending commands to the MCU. The
interface will be over TCP or a named pipe.

Web interface
-------------

The backend of the web layer reads from the sqlite3 database and displays
useful graphs on a web page. The graphs are rendered client-side using
jqplot, a jquery plugin. All of the javascript libraries are accessed over
cdn, meaning they don't have to be stored on the web server.

The backend is written in Python3, and uses the [Bottle web framework][bottle].
Bottle is a WSGI compliant framework, so it can be used with apache or nginx.
It can also run on its own using the builtin Python WSGIServer. This is the
current setup, and it can be run using

    $ web/webapp.py

where it serves the files to localhost:8080.

[bottle]: http://bottlepy.org/docs/stable/

The web page should also contain some commands to directly control the MCU.
These will be sent via the web backend to the logging daemon and from there
on to the MCU.

Circuit
-------

In brief: The Arduino nano plugs into the circuit board, reads its inputs from I2C, sending serial out via a MAX232. The heaters and pump are controlled via zero crossing triac driver and triacs. The filler solenoid is controlled via logic level fet. All outputs can be disabled, overwridden, or be controlled via MCU with a three position toggle switch. LED's indicate the output state, and the MCU flashes politely when operating. DC power is supplied by a 12V smps.

The circuit and artwork are in ```cad``` and ```eagle``` respectivly.

Issues
------
* The I2C driver needs a major upate which must happen in tandem with this (and all other dependent projects) to support a separate error state with recovery.
* If an I2C error in level is detected it must:
    1. Report 0mm
    2. Turn off heaters
    3. Turn off filler
    4. Turn off pump
    5. Raise a _level_ error flag in USART output
    
* If an error in temperature is detected it must:
    1. Report 0deg cent.
    2. Turn off heater
    3. Raise a _temperature_ error flag in USART output
    
Normal operation must continue once normal sensing is restored.

* Fill time detection and auto off. Sometimes the filler does not turn off correctl due to I2C errors or misreading. To avoid a flooded yard whilst on holiday a maximum fill time needs to be obeyed, taking into account a complete fill _with_ the pump on. If this time (`MAX_FILL_TIME`) is exceeded the filler must be disabled and not re-enabled unless done so manually via USART.



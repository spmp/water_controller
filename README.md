Water Controller
================

The aim of this project is to control and monitor a water heating system.
This is currently a **Work in Progress**, and it is largely incomplete.

The project consists of three main parts:

  1. An atmega328 MCU with a control system that directly interfaces with
     the water heater. It also operates over a serial interface to interact
     with a PC.

  2. A daemon running on the PC which can talk to the MCU. The daemon receives
     regular logging information from the MCU which it saves to a database.
     The daemon also allows various commands to be sent to the MCU.

  3. A web application that can display logs from the database, and can
     control the MCU via the daemon.

MCU
---

In development I use an Arduino Duemilanove, which has been a useful testbed
so far. The board takes its power directly from the USB cable attached to
the dev PC, so setup is minimal. To run the project, you will need to do the
following:

 1. Plug in the MCU via USB to your computer. Check that `lsusb` shows the
    device, and that /dev/ttyUSB0 is writable (add your user to that device's
    group if not).

 2. Install the avr gcc toolchain. In Ubuntu/Debian, the packages you need
    (at least) are `avr-gcc`, `avr-libc`, and `avrdude` and their
    dependencies.

 3. Run `make` to compile the source, and `make upload` to compile and upload
    to the MCU via `/dev/ttyUSB*`. The program will start running immediately.

At present, the MCU reads data from the following inputs:

 * 10 bit analog-in on ADC3
 * ultrasonic distance sensor (HC-SR04)
 * _more to come_

These are sent over the serial interface to the PC along with a "timestamp" (the
number of seconds since the program started) at regular intervals.

To view the output, run `screen /dev/ttyUSB0 57600` (exit with `C-a k y`).

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


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

Logging daemon (_Not started_)
------------------------------

This should save the data sent from the MCU to a sqlite3 database. It should
also have an interface for sending commands to the MCU. That will be over
TCP or a named pipe. Written in Python3.

Web interface (_Not started_)
-----------------------------

This needs to display useful graphs on a nice and pretty web page. It will
read from the sqlite3 database to supply the graph data, and the graphs will
be rendered client-side using some modern js chart api.

The backend will be written in Python3 and serve web pages using one of, say,

 * pure Python3 web server
 * apache + mod\_wsgi
 * nginx + uwsgi

Basically pick whichever one is easiest.

The web page will also contain some commands to directly control the MCU. These
will be sent via the web backend to the logging daemon and then to the MCU.


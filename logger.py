#!/usr/bin/python3

"""
data in: [timestamp, metric, value]

basically just dump that into a sqlite3 table.
"""

import sqlite3
import pyuv
import sys
import os
import serial
import signal
import time
import termios
import fcntl
import struct
import re
import datetime

class Logger:
    def __init__(self, dbname):
        print("Initialising logger database")
        
        self.conn = sqlite3.connect(dbname)
        self.cur = self.conn.cursor()

        #self.cur.execute("""
            #create table if not exists source (
                #name varchar(20)
            #)""")

        self.cur.execute("""
            create table if not exists hotwaterdata (
                time timestamp,
                temperature real,
                level real,
                volume real,
                pumpState integer,
                fillState integer,
                heatState integer,
                tth real
            )""")
        
    #def secondsConvertHMS(self, seconds):
        
        
    def add_data_point(self, timestamp, temperature, level, volume, pumpState,
                       fillState, heatState, tth):
        print("adding", timestamp, temperature, level, volume, pumpState,
              fillState, heatState, tth, "to db")
        self.cur.execute(
            'insert into hotwaterdata values (?, ?, ?, ?, ?, ?, ?, ?)', 
            (timestamp, float(temperature), float(level), float(volume), 
             int(pumpState), int(fillState), int(heatState))
        )

        self.conn.commit()

    def add_from_string(self, st):
        rx = "l\s+(?P<time>\d+)\st\s(?P<temperature>\d+) l (?P<level>\d+) v (?P<volume>\d+) p (?P<pump>\d) P \d f (?P<fill>\d) F \d h (?P<heat>\d) H \d OP \d+ T2H (?P<tth>\d+)"
        m = re.match(rx, st)
        if m is None:
            print("unable parse data_point:", st)
            return
        self.add_data_point(
            m.group('time'), m.group('temperature'), m.group('level'),
            m.group('volume'), m.group('pump'), m.group('fill'),
            m.group('heat'))


if __name__ == '__main__':
    l = Logger('log_timeseries.db')
    '''
    l.add_from_string(b"a 1234 14000 800")
    l.add_from_string(b"a 1235 14200 700")
    l.add_from_string(b"a 1236 13000 600")
    l.add_from_string(b"a 1237 14003 800")
    exit()
    '''

    loop = pyuv.Loop.default_loop()

    ser = serial.Serial()
    ser.port = '/dev/ttyUSB0'
    ser.baudrate = 38400
    ser.open()
    
    print("Serial port opened ", file=sys.stderr)

    tty_atmega = pyuv.TTY(loop, ser.fileno(), True)
    tty_stdin = pyuv.TTY(loop, sys.stdin.fileno(), True)
    tty_stdout = pyuv.TTY(loop, sys.stdin.fileno(), False)
    mybuf = b''

    def listen_atmega(tty, data, error):
        global mybuf

        mybuf += data.replace(b'\x00', b'')

        partial_lines = mybuf.split(b'\r\n')
        mybuf = partial_lines[-1]

        for line in partial_lines[:-1]:
            l.add_from_string(line)

        # tty_stdout.write(data)

    def listen_stdin(tty, data, error):
        if data is None or data == b"exit":
            loop.stop()
        else:
            tty_stdout.write(data)

    def listen_signal(handle, signum):
        print()
        loop.stop()

    tty_atmega.start_read(listen_atmega)
    tty_stdout.start_read(listen_stdin)

    signal_h = pyuv.Signal(loop)
    signal_h.start(listen_signal, signal.SIGINT)

    loop.run()

    ser.close()
    tty_stdout.close()
    tty_stdin.close()

    pyuv.TTY.reset_mode()
    l.cur.close()


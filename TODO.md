TODO
=========
for the
Solar hotwater controller
----


Reliability/Bugs
====
* Serial communications ceases, suspect I2C
  * Watchdog timer **Continued tests and not working!**
  * I2C comms that timeout
    * modify I2C routines to return status. If status is bad then disable associated
      hardware (i.e pump and filler) until a test proves them to be good.
    * Recheck status of all failed elements every _long_ time period

Features to add
====
* A reset flag that the daemon should set to see if its missed a reset
* I2C initialisation returns a status
* I2C initialisation (in 'hardware.c') checks for pressure sensor and follow
    similar pattern to temperature sensor - enabling/disabling pump/fill/heat
* Program over serial (Autoreset/manual reset?)
* EEPROM:
  * Saving of configuration periodically
  * Loading of configuration

Features to improve
====
* Reset level on fill-to-top to agreed greatest level. (maybe)
* Filler maximum time
* State limit checking code that takes into account failed readings
* Long time period checks of sensors if errors present.
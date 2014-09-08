TODO
=========
for the
Solar hotwater controller
----


Reliability/Bugs
====
* Serial communications ceases, suspect I2C
  * Watchdog timer
  * I2C comms that timeout
    * What do we return when they do?

Features to add
====
* Startup/initialisation text that includes version number etc.
* A reset flag that the daemon should set to see if its missed a reset... available via log.c
* Program over serial (Autoreset/manual reset?)
* EEPROM:
  * Saving of configuration periodically
  * Loading of configuration

Features to improve
====
* Reset level on fill-to-top to agreed greatest level.
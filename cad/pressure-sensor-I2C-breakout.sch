EESchema Schematic File Version 2  date Mon 24 Feb 2014 17:03:24 NZDT
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:state-machine-library_kicad
EELAYER 25  0
EELAYER END
$Descr A4 11700 8267
encoding utf-8
Sheet 1 1
Title ""
Date "23 feb 2014"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Connection ~ 7250 2200
Wire Wire Line
	7250 2200 7250 2850
Wire Wire Line
	7250 2850 7200 2850
Connection ~ 7450 2750
Wire Wire Line
	7450 2700 7450 2750
Connection ~ 3550 3400
Wire Wire Line
	3400 3400 7650 3400
Wire Wire Line
	4750 2450 4950 2450
Wire Wire Line
	8100 2900 7550 2900
Wire Wire Line
	7550 2900 7550 2750
Wire Wire Line
	7550 2750 7200 2750
Connection ~ 6700 2200
Wire Wire Line
	6700 2300 6700 2200
Connection ~ 6700 3400
Wire Wire Line
	6700 3300 6700 3400
Connection ~ 6000 3400
Wire Wire Line
	6000 2950 6000 3400
Wire Wire Line
	8100 2700 7650 2700
Connection ~ 7650 2900
Connection ~ 7400 2200
Wire Wire Line
	7400 2200 7400 2650
Connection ~ 4000 2200
Wire Wire Line
	4000 2650 4000 2200
Connection ~ 4000 3400
Wire Wire Line
	4000 3400 4000 3050
Connection ~ 5550 3400
Wire Wire Line
	5550 3400 5550 3050
Wire Wire Line
	3550 3400 3550 3050
Wire Wire Line
	7650 3400 7650 3000
Wire Wire Line
	7650 2700 7650 2200
Wire Wire Line
	3550 2200 3550 2650
Connection ~ 4750 3400
Wire Wire Line
	5550 2450 5550 2650
Connection ~ 5550 2450
Connection ~ 4550 2200
Wire Wire Line
	7400 3050 7400 3400
Connection ~ 7400 3400
Wire Wire Line
	7650 3000 8100 3000
Connection ~ 7650 2800
Wire Wire Line
	5450 2450 6000 2450
Wire Wire Line
	6000 2450 6000 2650
Wire Wire Line
	6450 3300 6450 3400
Connection ~ 6450 3400
Wire Wire Line
	6450 2300 6450 2200
Connection ~ 6450 2200
Wire Wire Line
	7300 2650 7200 2650
Wire Wire Line
	7300 2650 7300 2800
Wire Wire Line
	7300 2800 8100 2800
Wire Wire Line
	4550 2200 4550 2450
Wire Wire Line
	4750 3150 4750 3400
Wire Wire Line
	7650 2200 3350 2200
Connection ~ 3550 2200
Wire Wire Line
	7200 2650 7200 2600
Wire Wire Line
	7200 2950 7200 3400
Connection ~ 7200 3400
Text Label 7450 2700 0    60   ~ 0
SDA
Text Label 7200 2600 0    60   ~ 0
SCL
Text GLabel 3350 2200 0    60   Input ~ 0
Vss
Text GLabel 3400 3400 0    60   Input ~ 0
GND
$Comp
L CONN_4 P2
U 1 1 530999F2
P 8450 2850
F 0 "P2" V 8400 2850 50  0000 C CNN
F 1 "CONN_4" V 8500 2850 50  0000 C CNN
	1    8450 2850
	1    0    0    -1  
$EndComp
$Comp
L CONN_4 P1
U 1 1 530997F9
P 8000 2850
F 0 "P1" V 7950 2850 50  0000 C CNN
F 1 "CONN_4" V 8050 2850 50  0000 C CNN
	1    8000 2850
	1    0    0    -1  
$EndComp
$Comp
L C C4
U 1 1 5309976E
P 7400 2850
F 0 "C4" H 7450 2950 50  0000 L CNN
F 1 "0.1uF" H 7450 2750 50  0000 L CNN
	1    7400 2850
	1    0    0    -1  
$EndComp
$Comp
L C C3
U 1 1 53099624
P 5550 2850
F 0 "C3" H 5600 2950 50  0000 L CNN
F 1 "0.33uF" H 5600 2750 50  0000 L CNN
	1    5550 2850
	1    0    0    -1  
$EndComp
$Comp
L C C1
U 1 1 5309961B
P 3550 2850
F 0 "C1" H 3600 2950 50  0000 L CNN
F 1 "1uF" H 3600 2750 50  0000 L CNN
	1    3550 2850
	1    0    0    -1  
$EndComp
$Comp
L C C2
U 1 1 5309943C
P 4000 2850
F 0 "C2" H 4050 2950 50  0000 L CNN
F 1 "0.01uF" H 4050 2750 50  0000 L CNN
	1    4000 2850
	1    0    0    -1  
$EndComp
$Comp
L R R1
U 1 1 53099200
P 5200 2450
F 0 "R1" V 5280 2450 50  0000 C CNN
F 1 "750" V 5200 2450 50  0000 C CNN
	1    5200 2450
	0    -1   -1   0   
$EndComp
$Comp
L LTC2483 U2
U 1 1 5309912A
P 6500 2800
F 0 "U2" H 6500 2700 60  0000 C CNN
F 1 "LTC2483" H 6500 2900 60  0000 C CNN
	1    6500 2800
	1    0    0    -1  
$EndComp
$Comp
L MPXV-PRESSURE-SENSOR U1
U 1 1 53099121
P 4650 2800
F 0 "U1" H 4650 2800 60  0000 C CNN
F 1 "MPXV-PRESSURE-SENSOR" H 4800 2700 60  0000 C CNN
	1    4650 2800
	1    0    0    -1  
$EndComp
$EndSCHEMATC

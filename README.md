# RM3Switch
 A power monitoring device connected to the Blynk IO platform.
 Made to control a power generator with power input monitoring and scheduled timers to trigger relays.

-- Device Setup -- 
A Wemos D1 Mini
2 Optocouplers for power source input monitoring.
3 5v Relays
A voltage divider at the analog input 
to monitor the devince power source

Physical Relay 1    D1
Physical Relay 2    D2
Physical Relay 3    D5
Power Source Volt   A0
Source 1 input      D6
Source 2 input      D7

 --- Dashboard Setup ----
Power Source 1 LED  V6
Power Source 1 LED  V7

Relay vButton 1     V16
Relay vButton 2     V17
Relay vButton 3     V18

Calibrate Input1    V36
Calibrate Button1   V37
Calibrate Voltage   V38
Calibrate Button2   V39

Last Run Widget     V40
Meter 1 Widget      V41
Meter 2 Widget      V42


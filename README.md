# Drone_Receiver

Control 4 ESC connected to brushless motors through [pigpio library](http://abyz.me.uk/rpi/pigpio/cif.html).
Receive commands from the [Transmitter](https://github.com/EViallet/Drone_Transmitter) via wifi.

System output is mesured using MPU6050 gyroscope via [this awesome library](https://github.com/richardghirst/PiBits/tree/master/MPU6050-Pi-Demo).

PPM sequence at 1.0 ms pulse (lowest/arming pulse)
<img src="piscope.PNG" height="450" width="800">

# Working
* Wifi connection and transmission
* Writing to the motors

# TODO
* PIDs tuning
* Maybe send back some infos to the transmitter, like current angles and/or speed (3D model & motors speeds in real time on controller?)

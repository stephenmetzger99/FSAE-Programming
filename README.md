# FSAE-Programming
Louisville FSAE Programming Team

Contributors: Stephen Metzger, Nico Mitchell, Victor Papyshev

Objectives:
    -    Create a Steering wheel that uses arduino to recognize upshift and downshift signals from a set of paddle shifters
    -    Create a dashboard that displays vehicle data
    -    Create a data logging and telemetry system
    
    

Paddle Shifters:
  Shift up with the right paddle, shift down with the left paddle. The shifters control a relay which sends power to a pneumatic pump that    actuates the gear shift
Screen:
  Display MPH, RPM, and Current Gear
 Telemetry:
  allows us to study vehicle data (using python pandas/tensorflow/keras libraries)
  
 RPMs:
  RPMs will be retrieved using a Hall Effect sensor to calculate the RPMs on the crankshaft of the car.
  RPMs will be displayed on screen and using an led strip with lights incrementing as RPMs increase.


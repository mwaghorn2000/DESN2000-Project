# DESN2000-Project
A touch screen home automation system for guests staying at our Jindabyne cottage.

## Features
The Doorbell is implemented on the non debounced button and when pressed, the speaker will output a "ding dong" sound. This feature can be found in the doorbell.c file. 

The Automated Blinds use the light sensor to detect the light and adjust the position accordingly. Blind 2 will open between 9am and 5pm regardless of light level. Blind 1 will open based on the light level. The blinds position is indicated by the tricolour LEDs with the left being Blind 1 and the right being Blind 2. This feature can be found in the 

The Automated Smart Plug has the user set the time when they want their coffee. The smart plug will turn on 30 minutes before the specified time and 90 minutes after. The coffee icon in the coffee menu enables / disables the smart plug automation. The debounced button serves as a manual override and turns on / off the smart plug when in Sleep, Normal or Manual mode. This feature can be found in the plug.c file.

Operation modes are changed using the LCD screen. Each mode disables / enables the features. The "Away" mode disables all features. The "Sleep" mode only enables the smart plug. The "Normal" and "Manual" modes enable all features. This feature can be found in the main.c file and uses the sys variable.


# DESN2000-Project
A touch screen home automation system for guests staying at our Jindabyne cottage.

## Features
- Doorbell:
  - Implemented on the debounced button
  - When pressed the speaker will output a "ding dong" sound
- Operation modes
  - Each mode affect the automation settings of each feature
- Automated blinds
  - Reads light levels from the light sensor
  - When more light is detected, the blinds will open
  - When low light levels are detected, the blinds will close
  - The blinds location is indicated by the tricolour LEDs as mentioned in the project brief
  - The left LED indicates the position of Blind 1 and the right indicates the position of Blind 2
- Automated smart plug
  - User set time for when they want their coffee
  - The smart plug will turn on 30 minutes before the specified time and 90 minutes after
  - The non debounced button serves as a manual override and turns on / off the smart plug 


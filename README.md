# Chorded Keyboard Device

Firmware for an experimental Bluetooth chorded keyboard.  
Connects to [https://github.com/ids789/ChordedKeyboardAndroid](https://github.com/ids789/ChordedKeyboardAndroid)

##### Goal
Build a phone or tablet input device for use while flying a plane.  To be used for noting down air traffic control instructions, making navigational notes and doing calculations.  
* Must take up little space so that it can fit in a small cockpit.  
* Must be able to type quickly and comfortably, even during turbulence.  
* Must be able to use it without looking and when under high work load while doing other tasks.  

##### Concept
Create a small single handed [Chorded Keyboard](https://en.wikipedia.org/wiki/Chorded_keyboard). 
- Have a button for each finger allowing for 31 unique inputs to be available from only 5 keys.  
- Use full sized keyboard keys retaining the feel of a full sized keyboard in a much smaller form factor. 
- Connect to a phone wirelessly over Bluetooth LE so the phone can be placed high up in the field of view.  


### Implementation
##### Firmware
- Connects to a phone or tablet using Bluetooth LE over a custom GATT service.  Using a custom service as opposed to a HID keyboard service allows the phone to handle what each chord is and what they do providing much better flexibility for experimentation.  
- When a key is pressed the keyboard will wait until all keys are released before sending the chord.  The chord is sent as a 5 bit number where each bit represents each different key.  
- Pressing the power button switches the keyboard off by putting the microcontroller into a low power mode.  The keyboard will also sleep after 5 minutes of inactivity,then pressing any key will wake it up.  (it can power up and reconnect to a Blueooth device very quickly)
- The status LED flashes to indicate that it is waiting for a device to connect and is solid ON to indicate that it has connected to a Bluetooth device.  

##### Hardware:
- Based on the Nordic Semiconductor NRF52840 microcontroller, currently on an Adafruit Feather Express development board.  
- Buttons use Cherry key switches from an old mechanical keyboard, which provide good tactile feel when pressing complex chords.  
- Powered from a 400mAh LiPo battery, which integrates with the charging hardware built into the Adafruit board and lasts for about a week of regular use.    
- An combo power button and LED indicator is used to show the state and put the microcontroller into sleep mode.  

Device | Port
-------|-----
Button 1 | P0.04 (A0)
Button 2 | P0.05 (A1)
Button 3 | P0.30 (A2)
Button 4 | P0.28 (A3)
Button 5 | P0.02 (A4)
Power Button | P0.03 (A5) 
Status LED | P0.07 (D6)
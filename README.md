# Rinsense
A hygienic device!


## Circuitry

You can find the eagle schematic in the circuitry folder, as well as a PNG if you don't want to use eagle. All components used are fairly common and can be found at amazon or aliexpress for little money. I used a 5x7 perfboard sliced down the middle.

Parts list:

Main circuit:

| Name | Description | Amount | Example in a store |
|---|---|---|---|
| B+ / B- | DC jack | 1 | https://www.aliexpress.com/item/32760333770.html |
| C1 | Capacitor 0.1UF | 1 | Any electronics store |
| IC1 | ATTINY13A | 1 | Any electronics store. The through hole variety. |
| D1-D5 | RGB LED Common Anode 5MM | 5 | Any electronics store. |
| LS1 | Standard buzzer | 1 | https://www.aliexpress.com/item/32451629272.html |
| IR-RCV | 3MM IR Receiver | 1 | Any electronics store. |
| IR-SND | 3MM IR Emitter | 1 | Any electronics store. Should match the wavelength of the receiver. |
| Q1-Q3 | TN0606 N-FET or similar | 3 | I bought mine off of digikey. Any N-FET that matches specs will work tho. |
| R1 R3 R4 R5 R6 | 270 ohm resistor 1/8W or better | 5 | Any electronics store. |
| R2 R7 R8 R12 R13 | 220 ohm resistor 1/8W or better | 5 | Any electronics store. |
| R9 | 120 ohm resistor, 1/4W or better | 1 | Any electronics store. |
| R10 | 100k ohm resistor | 1 | Any electronics store |
| R11 | 100 ohm resistor | 1 | Optional. Can lower or increase to limit the beeps. Budget accordingly. |

Battery pack:
| Name | Description | Amount | Example in a store |
|---|---|---|---|
| DC Plug | DC contact that can be screwed in (You only need the metal part) | 1 | https://www.aliexpress.com/item/32917080229.html |
| Battery | LiPo battery 40x30x10 | 1 | https://www.aliexpress.com/item/33006283852.html |


Charger:
| Name | Description | Amount | Example in a store |
|---|---|---|---|
| DC screw jack | Standard box jack | 1 | https://www.aliexpress.com/item/32760333770.html |
| LiPo Charge board | Charger for 1S LiPo batteries | 1 | https://www.aliexpress.com/item/32650239370.html |


Additional components:

* Wire
* Solder
* Hot glue




## Code

Compile using Arduino. Make sure you burn the bootloader beforehand. I set the internal clock to 1MHZ which is enough for this. No external libraries are needed.


## 3d Printing

* Circuitry contains subfolders to the meshes.
* Folders prefixed with mmu_ are multi-material prints. You can ignore the colored parts of them if you don't have an MMU printer, and use stickers instead. Though for the charger I suggest using a transparent filament so you can see the LED indicator. The charger was designed for one of these boards https://www.aliexpress.com/item/32650239370.html
* I didn't need any supports while printing. Using layer height 0.15MM


### Main box:

* Start by putting the IR diodes into the holes at the bottom of the front plate. Then put the DC plug in the square cutout on the left side, note the cable channel.
* You can use hot glue to keep the DC socket in place.
* Put the circuit into the transparent section and slot it into the front plate (there are bevels at the bottom that it slots into)
* Put the back on top, making sure not to squeeze any cables.
* Add M3 screws into the two screw holes to keep it together

### Battery box:

* Remove the metal part from the plastic of the DC plug.
* Screw it into the hole from the front.
* Solder the LiPo battery onto the DC plug, be mindful of which one is positive and negative (positive should go on the inside, and negative on the outside).
* Glue the battery in so it stays.
* Snap the lid on.

### Charger:

* Slot the DC jack in
* Slot the charge module in so the USB port points towards the hole.
* Add hot glue to keep it together.
* Snap the bottom plate on.



This device was made as part of the code vs covid19 hackathon. 

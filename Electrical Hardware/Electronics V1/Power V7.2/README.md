# 12V Version 7.2
Power Control Board for microcontroller projects, designed at OPEnS Lab OSU.

The board use a [D-Flipflop](https://www.digikey.com/product-detail/en/texas-instruments/SN74LVC1G175DBVR/296-17617-1-ND/706599) to store the current state of the system.

| D-Flipflop     |
|:-------------:|
| <img src="https://user-images.githubusercontent.com/48141945/64284820-458f7780-cf0f-11e9-9bac-2621bce67374.png" width="90%" /> |

| Schematic Name | Pin Name                  |
|----------------|---------------------------|
| `Q`            | Input - tied HIGH         |
| `D`            | Output to power regulator |
| `CLK`          | Clock                     |
| `!CLR`         | Clear - Reset             |

The flipflop IC is work based on raising, positive edge of the clock (posedge)
The input `D` is always tied HIGH.
The `CLK` is normally pulled low until triggered by the Feather.

> In the original sleep state:
 * `D` is HIGH
 * `Q` is HIGH
 * `CLK` is LOW
 * `CLR` is HIGH

> when the RTC fires the alarm:

`CLR` (reset) is pulled down to GND -> make the output `Q` LOW
		-> Turn on the power regulator. The state remain the same since there is no change in the clock
		
> When Feather fired the shutdown signal:

`CLK` (reset) goes low->high which copy the input `D` to the output `Q`. Since `D` is always high,
		the output `Q` will change to HIGH -> shutdown the voltage regulator

System stay as sleep until the RTC fires again -> reset the flipflop
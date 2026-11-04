# BU2090F LEDS control with Arduino (LG FB162)

## Description of the project

This project aims to give a second life to a **LG FB162 NUMERIC CD LECTOR** front board and, therefore, control the leds with an **Arduino UNO R3**.

The scripts contains multiple useful things to control the leds, by sending packets to the leds controller (**BU2090F** - IC303) on the board. It also contains multiple pre-made leds animations, that will be detailled in the next readme update I hope.

## Circuit connectics explanations

The board circuitry is still intact, so we will connect our Arduino to the led controller instead of manually controlling each leds. This allows us to reduce the electronic work that I am not very familiar with. But we still need to solder 4 wires on the board.
The 4 pins we need on the board are :
- **5.6V** (pin 5 of the PN301 29-pin cable)
- **D-GND** (pin 6)
- **EXP_DATA2** (pin 10)
- **EXP_CLK2** (pin 11)

I feed the **5V** & **GND** from the arduino directly into the **5.6V** & **D-GND** of the board, and the 2 other pins are data wire to control the behaviour of the **BU2090F**, and so, lighting up the leds we want when we want.

## BU2090F packet & precise led control explanations

The **BU2090F** chip uses a 12 band specific synchronized packet to control the leds. This is implemented inside the *leds.ino* ``sendBU2090()`` method.

> To analyze this packet and help debugging I also wrote a processing script (oscillo.pde) that will receive some data from the serial connection opened by the *leds.ino* script. For this to work you need processing installed and you need to wire the **DATA_PIN** with the **DATA_OSC_PIN**, and **CLOCK_PIN** with the **CLOCK_OSC_PIN**. The leds script reads the digital values of the *_OSC_PIN and then send them to the oscillo script through the serial tunnel. If you see only single peaks on the oscillo you need to raise (A LOT) the 4 delay values that affects the BU2090F packet timing. Multiply them by 100 should be enough. But anyway this is only for debugging purpose you don't even need this to work.

The packet sent through this method is a ``uint_16_t`` with the last 12 bits representing the 12 bands to send. Because my FB162 board only has 10 leds, bit 5 and 11 are useless. And so I wrote some useful conversions methods that allow me to easily create the right ``uint_16_t`` to send to the **BU2090F** for only lighting the leds I want.

So the animations are made of struct ``Plan``, defined in *Plan.h* and then translated to the right ``uint_16_t`` that itself is sent to the chip with the right signal timings.
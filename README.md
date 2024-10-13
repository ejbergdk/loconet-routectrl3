# loconet-routectrl3

Route controller for a loconet-enabled model railroad.

This project is intended to help automating a model railroad, without having a computer attached.
It can read feedback signals (from contact tracks on the layout, or other forms of input),
and perform actions like setting signals and switches/turnouts/points.

The possibilities range from the simple, like setting a signal to red once a train passes it,
to the complex, like queueing up routes from one place on the layout to another with interlocking,
so conflicting routes can't be activated at the same time and trains won't collide. Well, at least in theory.

The requirements are a model railroad layout that uses Loconet, and the layout must have signals that can stop the trains.
This controller does **not** control the locomotives directly, only through signals.

Some knowledge of AVR microcontrollers and C code is required.

This project is made with Microchip Studio (formerly Atmel Studio). It does **not** work with Arduino.
The code is tested on a [AVR128DA48 Curiosity Nano](https://www.microchip.com/Developmenttools/ProductDetails/DM164151),
but it should work on the entire AVR DA family, and possibly the AVR DB family too.

Documentation is in the [wiki](https://github.com/ejbergdk/loconet-routectrl3/wiki).

### Note
This repository uses submodules. Add **--recurse-submodules** when cloning the project:

***git clone --recurse-submodules https://github.com/ejbergdk/loconet-routectrl3.git***

Alternatively, run ***git submodule update --init --recursive*** if you have already cloned the repo without it.

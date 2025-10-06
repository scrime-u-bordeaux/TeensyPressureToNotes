# Pressure sensor to MIDI notes controller

### Adapting the piano's double escapement principle to a (atmospheric) pressure sensor

Here you will find Arduino sketches designed to run on a Teensy board with an analog pressure sensor wired to the `A0` analog input.

This is a work in progress, the two approaches are already functional but with limitations due in corner use cases, and should be improved.
The idea is to trigger notes when the delta pressure goes above a certain threshold and use the max as the actual velocity, then, according to the pressure value when the threshold was crossed, define the lower pressure threshold values that will respectively allow retriggering a new note without stopping the actual one or stop the actual note (aka double escapement)
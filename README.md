# LED sunrise headboard project

Arduino project to control an RGB LED string using PWM from the ATMega328P and a MOSFET circuit per channel. The Arduino accepts Bluetooth commands via HC-05 module.

## Serial API
### lumos;
Set LEDs to warm white over 1 second

### nox;
Set LEDs to off over 1 second

### sunrise t;
Go through sunrise routine over t seconds

### rgb;
Print the current r, g, b values to serial. Useful for debugging via Arduino Serial Monitor. Requires the HEADBOARD_ASM_DEBUG flag to be set in code before compilation.

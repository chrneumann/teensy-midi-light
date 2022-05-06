# Teensy MIDI Light

A program for a Teensy board to controll a WS2812b LED stripe for use with DJ mixing software like Mixxx.

Tested with:

- [Mixxx](https://mixxx.org/) plus [MIDI Clock Output script](https://github.com/mixxxdj/mixxx/wiki/MIDI%20clock%20output)
- [Teensy 2.0 board](https://www.pjrc.com/store/teensy.html)
- WS2812b LED stripe (5m, 300 LEDS, 5V)
- XONE:K2 Controller

# Howto

- Change settings in program (#DEFINEs)
- Load program onto Teensy, configure Teensy as USB MIDI device (Libraries needed: FastLED, ...)
- Power stripe with external power source, connect digital input of stripe to digital pin of Teensy
- Enable MIDI Clock Output script for the MIDI device in Mixxx

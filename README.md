# Arduino Nano with DY soundboard Based Ghostbusters Proton Pack
Arduino Code for a Ghostbusters Neutrino Wand and Proton Pack
(Inspired from https://github.com/CountDeMonet/ArduinoProtonPack)

First I'm French speaking and doing my best to put this in English here, so sorry if sometime my sentences look strange or there is big faulty things around...

I'm in the process of building 4 proton packs for my sons. Since I have multiple packs to made, I do wnat to cut on prices and choose some most afforadable components. This design is greatly inspired form https://github.com/CountDeMonet/ArduinoProtonPack, but with cheaper audio board (DY-SV8F in my case) for sounds and shift register for the BAR GRAPH. The bar graph is made with a MAX7219 shif register. I also add 2 push buttons in the front wand adjuster to set volume. Those are the biggest changes made from the original design. I had to rewrite almost all the program because it took too much memory for a Arduino Nano (I did got warning from IDE after just making change to use DY board and MAX7219), and also because the DY-SV8F was sensible to multiple play calls and have some delay before a call end the actual state of the board causing some bugs in the sequences. 3d models for pack and wand are inpired from the 83% are base on the original references, but I have remake almost all the files and optimised them for my CR10s pro V2 with a 310 by 310mm bed. I'll publish to files when they are ready.

3d printed refrence models from the original design by https://github.com/CountDeMonet/ArduinoProtonPack :
The models are based off the following things on thingiverse: 
* Main base model for the pack: https://www.thingiverse.com/thing:2479141
* Base model for the proton wand: https://www.thingiverse.com/thing:1128019
* Full scale proton wand the 83% one is based on: https://www.thingiverse.com/thing:2334883
* And a clippard valve with text: https://www.thingiverse.com/thing:2286284

Electronic parts required for this build and code :

* 1x arduino nano. 
* 1x DC-DC buck converter 5A to convert 6V NiMh to 4.5V. Would also work with 12V battery pack. 
* 1x DY-SV8F playback triggered sound board with enough flashes for all the tracks if well compressed in MP3. 
* 1x Audio amp 
* 2x smalls speakers 3W, 4Ohms
* 1x MAX7219 shift register
* 1x Yellow Bar Graph, 10 leds
* 6x 7 WS2812 leds jewels for the cyclotron, vent and nose
* 1x 8 WS2812 leds light stick for the powercell 
* 5x Individual WS2812 for wand lights
* 3x toggle switches
* 2x push button
* 1x tactile button
* 1x rocker swith for power
* 1x battery pack 6v 3000 mAh  
* 1x NiMh charger module
* 1x relay 5V to switch between charging and powering the pack
* 1x mini USB female conector pcb to plug the pack for charging
* Mosfet and transistor for the auto power OFF circuit
* resistances and capcitors for circuits
* 2 small pcb boards for wand electonics
* 1 large pcb board forpack electronics

This is a work in progress, I'll update this while I can.

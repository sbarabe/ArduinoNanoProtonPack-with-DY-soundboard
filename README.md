# Arduino Nano with DY soundboard Based Ghostbusters Proton Pack
Arduino Code for a Ghostbusters Neutrino Wand and Proton Pack

(Design inspired from https://github.com/CountDeMonet/ArduinoProtonPack)

First I'm French speaking and doing my best to put this in English here, so sorry if sometime my sentences look strange or if there is big faulty things around this repository...

I'm in the process of building 4 proton packs for my sons. Since I have multiple packs to made, I want to cut on prices and choose some most affordable components.  
This design is greatly inspired form https://github.com/CountDeMonet/ArduinoProtonPack, but with cheaper audio board (DY-SV8F in my case) for sounds and shift register (MAX7219) for the BAR GRAPH. I also add 2 push buttons in the front wand adjuster to set volume. Those are the biggest changes made from the original design. I had to rewrite almost all the program because it took too much memory for a Arduino Nano (I did got warning from IDE after just making change to use DY board and MAX7219), and also because the DY-SV8F was sensible to multiple play calls and have some delay before a call end the actual state of the board causing some bugs in the sequences. 

3d models for pack and wand are inpired from the 83% are base on the original references, but I have remake almost all the files and optimised them for my CR10s pro V2 with a 310 by 310mm bed. I'll publish the files when they are ready. For now you can use any adapeted 3d printed refrence models from the original design by https://github.com/CountDeMonet/ArduinoProtonPack :
The models are based off the following things on thingiverse: 
* Main base model for the pack: https://www.thingiverse.com/thing:2479141
* Base model for the proton wand: https://www.thingiverse.com/thing:1128019
* Full scale proton wand the 83% one is based on: https://www.thingiverse.com/thing:2334883
* And a clippard valve with text: https://www.thingiverse.com/thing:2286284

If you're in no rush to make the electronics, you can buy from Aliexpress and wait to receive the parts while you're building the pack and wand frames...
If you are in a rush, you can find all this on Amazon, Digikey, Mouser, Robotshop, etc.
Electronic parts required for this build and code :

* 1x arduino nano 5V, atemga 328P : https://s.click.aliexpress.com/e/_ABoigL
* 1x DC-DC buck converter 5A to convert 6V NiMh to 4.5V. Would also work with 12V battery pack : https://s.click.aliexpress.com/e/_9R0vlv
* 1x DY-SV8F playback triggered sound board with enough flashes for all the tracks if well compressed in MP3 : https://s.click.aliexpress.com/e/_9Ix2f1
* 1x Audio amp 2x3W: https://s.click.aliexpress.com/e/_Afwd8X
* 2x smalls speakers 3W 4Ohms : https://s.click.aliexpress.com/e/_9vz9ur
* 1x MAX7219 shift register : https://s.click.aliexpress.com/e/_AlYLuJ
* 1x Yellow Bar Graph, 10 leds : https://s.click.aliexpress.com/e/_9uJiD5
* 6x 7 WS2812B leds jewels for the cyclotron, vent and nose : https://s.click.aliexpress.com/e/_9841JN
* 1x 8 WS2812B leds light stick for the powercell : https://s.click.aliexpress.com/e/_ADHJfz
* 5x Individual WS2812B for wand lights : https://s.click.aliexpress.com/e/_9f1dzp
* 3x toggle switches : https://s.click.aliexpress.com/e/_9iykgj
* 2x push button : https://s.click.aliexpress.com/e/_9HSwyb
* 1x tactile button : https://s.click.aliexpress.com/e/_A4rCzd
* 1x rocker swith for power : https://s.click.aliexpress.com/e/_9y4cvd
* 1x battery pack 6v 3000 mAh  : https://s.click.aliexpress.com/e/_9Q6bsB
* 1x NiMh charger module : https://s.click.aliexpress.com/e/_9HCKGT
* 1x relay 5V to switch between charging and powering the pack : https://s.click.aliexpress.com/e/_ACdvqb
* 1x mini USB female conector pcb to plug the pack for charging : https://s.click.aliexpress.com/e/_9gum55
* 1x P channel Mosfet like the FQP50N06 TO220 for auto power off :  https://s.click.aliexpress.com/e/_9IXkZN
* 1x 2N3904 N channel transistor for the auto power OFF circuit : https://s.click.aliexpress.com/e/_97Cq0F
* 2x Zener diode 1N4728 for auto power off circuit : https://s.click.aliexpress.com/e/_A9cVST
* Resistances and capcitors for circuit : https://s.click.aliexpress.com/e/_ApHJUJ
* Small Ceramic capcitors for circuit : https://s.click.aliexpress.com/e/_AdiVYn
* Electrolytic Capacitors for circuit : https://s.click.aliexpress.com/e/_9Hpgy3
* 2 small 20x80mm pcb boards for wand electonics : https://s.click.aliexpress.com/e/_9HLL3d
* 1 medium pcb board for pack electronics like this 5x9.5cm one : https://s.click.aliexpress.com/e/_9Hk8kF
* 2 RJ45 adapter board to join wand swithes to pack : https://s.click.aliexpress.com/e/_9ia8hd
* 2m of Ethernet Cable with plug (NOT crossover) : https://s.click.aliexpress.com/e/_9jk9Dt
* 2m of shield 3 wires cable for pack towand shift register : https://s.click.aliexpress.com/e/_A1atPV 
* 1 short audio cable 3.5mm male jacks, 3 wires, to be cut in half to link DY-SV8F to PAM amp : https://s.click.aliexpress.com/e/_AYneqn

This is a work in progress, I'll update this while I can.

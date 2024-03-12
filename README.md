# SBK-Pack Ghosbusters Proton Pack for Kids, Arduino based electronics


INTRODUCTION :

First I'm French speaking and doing my best to put this in English here, so sorry if sometime my sentences look strange or if there is big faulty things around this repository... If you found spooky things I should correct, please let me know !

So the SBK-Pack is a Ghostbusters inspired proton pack model and electronics for children. It's about 80% sized of a regular pack and it includs motherboard and backpack structure. It's not screen accurate, but close enough. Screen accuracy have been trade for :
* Mostly all 3D printed parts ;
* Low costs sourcing ;
* Easy assembling ;
* Easy finishing : just primer and paints for an acceptable good looking proton pack, but you could always put putty and sand those parts if it's what you like;
* Practical and functionnal.

Kids don't really care if it's 100% screen accurate or sanded finished, but they do care that it's is sturdy, great looking, noisy and flashy.

As it's said, it's a kid size model, if your looking for an adult size screen accurate proton pack, this great model migth be your wat to go : https://github.com/mr-kiou/q-pack



HISTORY :

I was looking to build a few kids size Ghostbuster proton pack for my children and I found it hard to source and produce for a relative low budget. Then I found CountDeMonet 3D model on Thingiverse and his tutorial/repository, it help me a lot but I wanted to push it further. So the design here is inspired from the great work made here : Design inspired from https://github.com/CountDeMonet/ArduinoProtonPack.

 
  
3D MODELS :

The SBK-Pack is about 80% sized, I say about because it's not a downsize model, but all the part have been redesign to achieve easy printing, assembling and finishing.

The models are not 100% screen accurate and they are inspired from the movies and others available 3D models like those :
* Main base model for the pack: https://www.thingiverse.com/thing:2479141
* Base model for the proton wand: https://www.thingiverse.com/thing:1128019
* Full scale proton wand the 83% one is based on: https://www.thingiverse.com/thing:2334883
* And a clippard valve with text: https://www.thingiverse.com/thing:2286284
  
There is 2 models available, an GB1/GB2 inspired model, and a Frozen Empire model. Someone migth want to mix botch and create an Afterlife model...

Most of the parts can be printed on a medium sized bed, like Prusa Mk3 or Ender3 printer, but the synchronous generator require for now a 300x300 printer bed. A split part should be produce for it eventually...
but I have remake almost all the files and optimised them for my CR10s pro V2 with a 310 by 310mm bed. 

  
  
SCHEMATICS :

PLEASE TAKE NOTES : I've provided Fritzing schematics for reference. Since I'm not an electronic designer, they migth be some error in components sizes, selections and locations. It's up to you to use those schematics as they are with or without corrections. I've build my prototype with those schematics, but it's not fully tested yet... Also, there is no circuit protection on those schematics, so it's something that can be improve there!
  
  
  Charging circuit :

  The charging circuit must be power by a 5V power supply of at least 1A, like a mobile from a mobile phone charger. Since 1A is enough to operate the charging module, but too low for the pack, there is a switching relay preventing the pack operation while charging. So when the charging module is powered, the relay is also powered and switch the supply to the battery only. When disconnected, the switching relay provides supply to the pack from the battery.
  
  This circuit can be ommited if you want to just plug you're battery pack without charging module.


  Auto power off circuit :
  
  To spare the battery, an auto power off circuit is integrated to shut down the pack if no activity is detected. The Arduino sketch will turn off the POWER OFF pin after a 5 minutes delay (or 15 minutes if themes are playing). Those delays can be modified in the sketch.
  
  To power the pack, main switch must be on, but you also need to press and maintain the fire button for a short time, aka the time it takes to power the Arduino Nano and to put the POWER OFF pin on high level (D12 on schematics). When this pin is set to low level, the pack power will be cut.
  
  If you want to omit this auto power off circuit, just plug the main switch into the step down buck converter and Arduino Vin.
    
    
  Pack powering :
  
  Except the Arduino Nano who should be power by 6V to 12V, all electronic on this schematics can be powered by 3.5 to 5V. 
  So power from the battery pack could be between 6V to 12V : Arduino Nano will be power directly from the battery pack.
  The rest should be power by a step down DC-DC buck converter ajusted to 4.5V.
  
  
  Resistors and capacitors :

  There are a few capacitors around these circuits to provide voltage surge/drop protection.
  There are also few resistors to protect elements from high voltage.
  I'll not explain the sizing here, so it's up to you to use the selected ones, other values, or none.


  CODE

  This design is greatly inspired form https://github.com/CountDeMonet/ArduinoProtonPack, but with cheaper audio board (DY-SV8F in my case) for sounds and shift register (MAX7219) for the BAR GRAPH. I have also added 2 push buttons in the front wand adjuster to set volume. Those are the biggest changes made from the original design. I had to rewrite almost all the program because it took too much memory for a Arduino Nano (I did got warning from IDE after just making change to use DY board and MAX7219), and also because the DY-SV8F was sensible to multiple play calls and have some delay before a call end the actual state of the board causing some bugs in the sequences.

  
ELECTRONIC PARTS LIST :

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

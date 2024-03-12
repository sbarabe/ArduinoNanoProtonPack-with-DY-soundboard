# SBK-Pack Ghosbusters Proton Pack for Kids, Arduino based electronics

![Elsa mod_00-Proton Wand V6 assy v8](https://github.com/sbarabe/SBK-KidSizeProtonPackArduino/assets/74213612/b9ed1225-d423-41d9-be64-e7c17c225ae0)
![Elsa mod_00-Proton Pack assy v5](https://github.com/sbarabe/SBK-KidSizeProtonPackArduino/assets/74213612/abb95de4-9516-46d4-8b80-1d9e26ce9118)

## INTRODUCTION :

The SBK-Pack is a Ghostbusters inspired proton pack model and electronics for children. It is about 80% regular pack sized, and it includes motherboard and backpack structure. It is not screen accurate, but close enough. Screen accuracy have been traded for:
* Mostly all 3D printed parts.
* Low costs sourcing.
* Easy assembling.
* Easy finishing.
* Practical and functional.

As it is said before, it is a kid size model, if you are looking for an adult size screen accurate proton pack, this great model might be your way to go: https://github.com/mr-kiou/q-pack

This is a work in progress, schematics and code updates should be expected.

## SUPPORT MY WORK

If you use and like my work, please support me to help me keep this project up to date : paypal.me/sbarab
You wouldn't believe how much work it is to make this kind of project available, well mounted and up to date !

Thank you !!!

## HISTORY:

I was looking to build a few kids size Ghostbuster proton pack for my children, and I found it hard to source and produce for a relative low budget. Then I found CountDeMonet 3D model on Thingiverse and his tutorial/repository, it helps me a lot, but I wanted to push it further. So, the SBK-Pack is initially inspired from the great work by https://github.com/CountDeMonet/ArduinoProtonPack.
  
## 3D MODELS:

The SBK-Pack is about 80% sized. It is not a downsize model, all parts have been redesigned to achieve easy printing, assembling, and finishing.

There are two models available here, an GB1/GB2 inspired model, and a Frozen Empire model. Someone might want to mix botch and create an Afterlife model...

The models are not 100% screen accurate, and they are inspired from the movies and others available 3D models like those:
* Main base model for the pack: https://www.thingiverse.com/thing:2479141
* Base model for the proton wand: https://www.thingiverse.com/thing:1128019
* Full scale proton wand the 83% one is based on: https://www.thingiverse.com/thing:2334883
* And a clippard valve with text: https://www.thingiverse.com/thing:2286284
* Q-pack full size complete models: https://github.com/mr-kiou/q-pack
  
### 3D printer size required

Most of the parts can be printed on a medium sized bed, like Prusa Mk3 or Ender3 printer, but the synchronous generator requires for now a 300x300 printer bed. A split part should be produced for it eventually...

### Printer/Slicer setup

Parts are not hollow if there is no need to it (like electronic needs). Solid parts are easier to print and less prone to artifacts and warping. It doesn't require more filament to print them as a solid if infill isen't too intensive. Three walls thickness with a 0.4mm nozzle should be ok for side wall. 

For better surface finish, and less finishing work, variable layers heights should be used if you master it. 

### Material

PETG is recommend over PLA because it is more flexible and temperature resistant: you do not want your precious proton pack to soften while taking a sunbath. But hey PLA is easier to print and have great finish.

### Non printed parts

There are a few non printed esthetics parts to complete the pack assembly:
- 3/4in PVC tubing and fittings (wand handles and backpack rack)
- corrugated plastic pipes
- 4mm pneumatic tubes
- 2mm electric wires
- Clear plastic test tube (neutrona wand tip)
- Clear round lens (cyclotron)
- Clear rectangular lens (wand body grid)
- Clear rectangular blue lens (PowerCell)
- Round pop-up lens (wand light indicators)
- Wood rod (ion arm rod) 
- Straps (backpack rack)
- Shoulder straps (backpack rack)
- Pipe foam (backpack rack)

The full part and sourcing list will be soon available with the assembly guide.

### Assembly

Model assembly is made with glue, screws, bolts, and nuts. Except for part you wish to be movable, fast acting epoxy glue in combination with screws are recommended for assembling parts together.

A full assembly guide should be available shortly.

### Finishing

If parts printed with variable layers heights and/or thins layers, primer and paints should be OK for an acceptable good looking proton pack. But if you want to push it, you could always use putty and sanding on all parts for a perfect finish. It is all up to you!

## ELECTRONICS:

There are special designed PCBs and part to fit these models beautifully. They save a lot of time and surely fits the space. They should be available soon.

But you could also make your own circuit based on the following schematic.
  
### Schematics

PLEASE TAKE NOTES: schematics are provided for reference, they should be check and corrected to your needs: power, component sizes and selections, noise filtering, protection, and features. It is up to you to use those schematics as they are with or without corrections. 

Schematics should be available soon. Meanwhile you can check this repository for a workable schematic: https://github.com/CountDeMonet/ArduinoProtonPack.
  
###  Pack powering
  
Except the Arduino Nano who should be power by 6V to 12V, all electronic on these schematics can be powered by 5V. 
So, power from the battery pack could be between 6V to 12V: Arduino Nano will be power directly from the battery pack.
The rest should be power by a step-down DC-DC buck converter adjusted to 4.5V.
  
###  Resistors and capacitors

There are a few capacitors around these circuits to reduce noise in power supply. 
There are also few resistors to provide right voltage to elements.
Sizing will not be explained here, so it is up to you to use the selected ones, other values, or none.


###  CODE

A specific code for these 3D models and schematics should be available soon!

Meanwhile, since it is an Arduino based project, you can also adapt another code to suit it, here are two other codes available:
- https://github.com/MikeS11/ProtonPack
- https://github.com/CountDeMonet/ArduinoProtonPack


  
  



  
  

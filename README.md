ArduinoReactionGame
===================

Assignment 9. ARDUINO PROGRAMMING - INTO

Authors:  
 Freyr Bergsteinsson <freyrb12@ru.is>  
 Egill Bjornsson <egillb12@ru.is>

<a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/3.0/deed.en_US"><img alt="Creative Commons License" style="border-width:0" src="http://i.creativecommons.org/l/by-nc-sa/3.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-nc-sa/3.0/deed.en_US">Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License</a>.

INTO_Skil9_EgillOgFreyr_ReactionGame
------------------------------------

This is a supplemental project for potential creativity points.
Play a reaction game.

Win the game by being quick on the button and reach 10 points.
When you start the game, you have 0 points. To get a point you need to
press the button on the board within 1100ms after the RGB led lights
up with a blue color. The light goes off at random, so you'll have to be
on your toes. If you don't press the button within this period, the
light goes red and you get a minus point. If you press too soon, this
will also happen. Sometimes the light that comes up isn't blue,
but red, in which case the player should not press the button, which
will result in a minus point.

If the player manages to press the button while the blue light is on,
the light goes green, a positive buzz is played and one point is received
(the 7-segment display will show 1).

After each reaction (red or green light), you do the above process again,
but now with a smaller reaction period. The reaction period then goes down
by 100ms for each point you get, so with 9 points you only have 200ms to
press the button and win the game!

[Video showing the assembled Arduino running this code](http://youtu.be/mJl5DEDiNxk).


Arduino Assembly
----------------

Accurate circuit diagram can be found [here](https://www.circuitlab.com/circuit/ex3a3m/into-skil9-egillogfreyr/).

In general terms:

    Pin | Connected to  
    ----+------------------------------------------------------  
      0 | Seven segment display pin 7  
      1 | Seven segment display pin 6  
      2 | Seven segment display pin 4  
      3 | Seven segment display pin 2  
      4 | Seven segment display pin 1  
      5 | Seven segment display pin 9  
      6 | Seven segment display pin 10  
      7 | 330 ohm resistor -> Seven segment display pins 3 & 8  
      8 | Button pin  
      9 | 330 ohm resistor -> RGB led red light pin  
     10 | 330 ohm resistor -> RGB led green light pin  
     11 | 330 ohm resistor -> RGB led blue light pin  
     12 | 10k ohm resistor -> Buzzer pin  
     13 | Not used  
    ----|------------------------------------------------------  
     5v | Buzzer voltage pin  
    ----|------------------------------------------------------  
    Gnd | RGB led ground pin  
    Gnd | Button ground pin  
    Gnd | Buzzer ground pin

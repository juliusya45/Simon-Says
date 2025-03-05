# Simon Says
After learning how to design schematics and PCBs on Altium I wanted to take what I learned to make my very own PCB. I wanted to start small, and after searching the web for ideas on what a beginner PCB project would look like, I saw someone had recreated the Simon game with a microcontroller, buttons, LEDs, and their own custom PCB. I wanted to do the same with the hardware I have which is what this project is.

# The Game
The Simon game is a very simple game that consists of 4 colored buttons, each with a corresponding LED. It is a memory game where the user is tested to see how many button presses they can memorize in a random sequence. So the computer will first light up one of the colored LEDs and the user has to repeat that color by pressing the corresponding button. Then the computer repeats that sequence, but this time adding another color (could be the same one) to the mix. The game keeps on going until the user fails to match the sequence the computer has generated.

# My Code:
I am using an ESP32 Supermini as the brains of my PCB. I plan to use the Arduino IDE to develop and upload code onto the chip.

So far I have created this flowchart for the game logic:

![Simon Says drawio](https://github.com/user-attachments/assets/c2ed2a09-3c20-45d6-88d3-0e0bc124afc0)

I have also coded a test version of my code that has been updated to this project. I plan on using interrupts in the future to prevent the chip from constantly polling for input changes. I also plan on using the FreeRTOS framework for the ESP32 to take advantage of task scheduling and to optimize my script.

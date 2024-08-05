# WRO-2024-SAPIENS
WRO-2024-SAPIENS

## Engineering Materials
All the materials used to create our robot
- 1 Arduino Nano Every (https://docs.arduino.cc/hardware/nano-every/)
- 1 TB6612 H-Bridge (HW-166)
- 4 Ultrasonic Sensors (1 at the front, 1 at the back, the other two right and left)
- 1 Small on/off switch
- 1 PushButton
- 1 Blue LEDS (1 for power, one for indication of the code)
- 2 resistors (360 Ohms) for code Indicator LEDS
- 25 Proto Wires
- 34 Rigid Wires (Hand cut)
- 2 Big Rigid Wires
- 2 Ceramic Capacitor 104
- 1 Capacitor 100 25-50V
- 1 Voltage Regulator L7805
- 1 Mini Power Supply HW-131 (Also works as voltage regulator) 5V and 3.3V2
- 1 Porta Fusibles
- 12V Battery 3S (18650 + BMS + 2 cable outings) (more about this can be seen in the electromehcanical components)
- 1 Chasis (DC Auto) This includes:
    - 4 Wheels
    - 1 Motor (more about this explained in other sections)
    - 1 Servo
- Multiple 3d pieces whih can be found in the following directory
[3d_files](models/3D_files/)

## Building instructions
- Chasis (Motors and base): https://mp.weixin.qq.com/s/dOADMsXeo7Lxebgg0yiY5Q
- Electronics (Circuit and battery): [The circuit diagram can be found in the schemes directory](schemes/)
- Code (For the arduino): [The source code can be found in the src directory](src/)

## Contents
- [Models](models/) This directory contains all used 3d models
- [Other](other/) This directory is empty for now, as no miscellancious files have had to be added
- [Process](process/) This directory will be completed for the national as it will contain the process that it took for us to get there
- [Schemes](schemes/) All wiring diagrams can be found in this directory
- [Source Code](src/) This directory contains all the code used in this proyect
- [Team Photos](t-photos/) The official Team photos can be found here
- [Performance Videos](p-videos/) The robot Performance videos for each challenge
- [Vehicle Pictures](v-photos/) All vehicle photos can be found in this directory, including some individual components
This is just a test to see if content linking works

## Introduction
Brief intro on our solution

For our robotics competition, we developed an innovative navigation solution that leverages Arduino's vision capabilities to detect and respond to color signals. By utilizing a camera module, our robot can identify green and red colors, which dictate its turning directions—left for green and right for red. This color-based navigation system is complemented by a comprehensive obstacle detection and avoidance mechanism.

To enhance our robot's awareness and maneuverability, we integrated four ultrasonic sensors—three positioned at the front and one at the back. These sensors provide real-time distance measurements, enabling the robot to detect and avoid obstacles from multiple directions. Our approach demonstrates the synergy of vision technology, ultrasonic sensing, and precise control algorithms, resulting in a responsive and reliable robotic solution.

## Mobility 
Our robot uses an Ackerman steering system, with this steering system we can make our robot do the turns that have to be done during the 3 rounds. But because of the shape of our chassis and the type of steering system we use, our robot can only turn up to a certain point. In the case of turning to the right, it can only turn 30 degrees and in the left one it can only turn 10 degrees.

Here you can see the shape of the chassis of our robot as a guide:
- [Chassis of our robot](v-photos/) 

Some of the problems we faced with the steering system is that we had to come up with a way for the robot to make the turns neither too tight nor too wide, to fix this we use ultrasonic sensors on the front, right and left side of the front of the robot that are detecting how far it started and how many degrees it has to move forward while it is turning. For example during the open challenge round we made the robot turn when one of the ultrasonic sensors on one of the sides stops detecting or detects a greater distance than it had detected during the straight line, the sensor that performs this action depends on whether it is the one on the right or left, when it is the one on the right is when the round is being performed clockwise and when it is the one on the left when it is counterclockwise. 

Meanwhile the servo motor is at 80 degrees the robot will turn to the left, so if it is at 120 degrees the robot will turn to the right, but you think that when the servo motor is at 100 degrees it will be in a position in which the wheels will be straight, this thought is incorrect because when the servo is at 100 degrees it will be slightly turning to the right, then we ask two questions, the first one is: So how many degrees should be the servo motor so that the robot wheels are straight? To answer this question, the degrees that the servo must be at for the wheels to be straight is 90 degrees, and this is where the second question comes in, which is: Why when the servo is at 90 degrees will the wheels be straight, if when the servo was at 80 degrees the robot would turn to the left and when the servo was at 120 degrees the robot would turn to the right, if the degrees that are in between these two numbers of degrees are 100 degrees? Well to answer this question, the 90 degree position is considered "straight" or "neutral" in many servo motor systems because it is the preconfigured standard in many servos to indicate that there is no deviation to the left or right and because of how we have the servo positioned with the chassis we are using and the Ackerman steering system our steering only goes from 80 to 120 degrees.

For the speed management part, our robot is generally going to go at 120 speed on straight lines, while it is rotating it will go at 100 of speed.

As for the equations we use 2 formulas, between the 2 formulas we have the turning time, this establishes that the speed that we are using during the turn, that is 100 We are going to divide it by 100 and the result we are going to multiply it by 1200 and this would give us the time during which we are going to rotate in milliseconds; the second formula is about the angle of rotation in this we are going to divide the speed by the result of the previous formula, then this result we are going to multiply it by 130 and the result that this formula gives us we are going to subtract it to another 130 and the result of this subtraction is going to give us the angle with which we are going to rotate. We use these formulas to make the turns between sections. 

## Strategy
Strategy Explanation
Color Detection with Arduino:
We integrated a Nicla vision with the arduino nano every to accurately detect colors. This system allows the robot to recognize green and red colors, which serve as signals to turn left or right, respectively. By using a camera module and appropriate color detection algorithms, the robot can continuously monitor its environment and respond in real time.

Directional Decision Making:
Upon detecting the color green, the robot is programmed to turn left. Conversely, when red is detected, the robot turns right. This color-based decision-making process ensures that the robot navigates efficiently and avoids obstacles or follows a designated path based on the predefined color signals.

Border Detection and Turning:
In addition to color detection, our robot is equipped with ultrasonic sensors to identify when and where it should turn. It uses an "umbral" which is a certain distance which if the robot pases, it will start to turn to a certain direction. That direction is decided by the other two ultrasonic sensors to the left and to the right.

Implementation Details
Code and Integration:
Our code, available here, outlines the specific algorithms and logic used for color detection, decision-making, and obstacle avoidance. The integration of the vision system with Arduino's control mechanisms is crucial for the robot's functionality.

Color Detection Algorithm:
The algorithm processes the camera feed to identify the presence of green or red. This involves filtering the image to isolate these colors and then using thresholding techniques to determine the direction.

Obstacle Avoidance Routine:
When the sensors detect an obstacle, the robot executes a series of steps to navigate around it. This includes stopping, scanning the surroundings, calculating an alternate path, and resuming its movement while continuing to rely on the color signals for direction.

Conclusion
Our solution leverages the capabilities of Arduino's vision system to create a responsive and efficient navigation strategy. By combining color detection with obstacle avoidance, our robot can intelligently navigate its environment, making it well-suited for the challenges of the competition.

This approach not only demonstrates the effectiveness of integrating vision with control systems but also highlights the importance of robust algorithms in achieving reliable performance in dynamic conditions.

## Code Explanation
In order to enact our strategy and enable our robot to complete each phase, our team created multiple programs that will be summarized and explained in the following text. For reference look at the [src directory](src).

The src directory is divided into 3 sections, Arduino Code, Nicla Code, and Other. This text will focus on the Arduino code as the Nicla is currently not being used. [The Arduino Code Directory](src/Arduino Code/) is divided in 2 sections, phase 1 and phase 2. At the time of writing, only phase 1 has been completed, that being said, our proyect is still in development, and in the near future it will be added to the github. 

[Phase 1](src/Arduino_Code/Phase_1):
Phase 1 contains multiple programs to show how our code has changed, the main code is called newcodephase1

- The program starts by defining all variables and importing all libraries, this include, all pins (except the two Led indicators), the max angle for the servo, and some other useful variables.

- Next is the PD, or Proportional Derivative. This PD is used later in "Accelerar Recto" to help mantain the robot in a straight line, there are multiple comments in the code explaining the PD, so the specific details will not be mentioned in this text. 

- After this comes the set up function, which as is said in the name, sets up the robot. It defines all pin modes and the servo. 

- Then comes the actual main part of the code, which can be found in the loop function. The loop function starts with a while loop that will repeat until a button is pressed. Pin #5 is in mode Input, which means that it recieves current. When the button is pressed, current from the battery is passed into the pin which causes the while loop to end. After the while loop ends, the 3 ultrasonic sensors measure the current distance, and each value is defined in a variable. This is done by calling the "medir_distancia" function which returns a float value.
  
- The medir distancia function can be found in line 241, it uses "cases" to determine which ultrasonic has to be activated. Once that is defined, the ultrasonic sends the sound waves and a duration value is returned, this value is processed by using the formula ((duration * 0.034 / 2)*0.95), which returns a distance reading in centimeters.

- Returning to the loop function, next up, a for loop starts. This for loop repeats 70 times, which is an absurd number of times right? Well thats the amount of times the function we use has to repeat in order to complete the 3 laps. Unfortunately, this is due to a weird error in our code that makes it so sometimes the turn function ends prematurely. 70 loops garantues that the robot completes the 3 laps, but in exchange, sometimes it does 3.5 or 4 laps, which means we can't consistently stop in the initial spot (this subtracts 3 points from our phase 1 score)

- Getting back to the functions used, first_turn is the function used for all turns, it was supposed to be only a function for the first turn, but it was so consistent we just used it repeatedly. First_turn is made up of 2 main parts, moving the robot forwards until it meets the umbral (or the left/right sensor detect more than 150 cms which indicated a turn), and then the actual turning.
- When the robot goes forward our pd constantly corrects the robot's position, we also have save crash function which makes it so if the robot is about to crash, it goes backwards and turns, preventing it from ending the round. This repeats until, how we mentioned earlier, the ultrasonic sensors forwards and back match the umbral or the right/left sensors detect more than 150 cms. It should probably be noted that the left anf right sensors almost never end the loop, and it is almost always the umbral.
- After the forwards loop ends, the robot detects right and left to decide what direction it will turn, the function left_turn and right_turn take in 2 parameters. Angle and time, angle being the angle at which it will turn, and time being the amount of time it will turn. Yet, if you see the actual code, you will notice that we set time at 0, so that means it shouldn't be turning right? Well, we found out that turning bya time was very inconsistent since gradual errors make the robot eventually crash, for this reason we are now using the left_end_turn function, which is incredibly consistent for 2 main reasons
- 1. It turns gradually from the specified angle (normally 60) to 84, increasing 1 degree every 112 miliseconds
  2. It turns, not by time, but until the ultrasonic sensors detect from a range of 100 to 30. This is the main reason why the robot is so consistent when turning compared to the previous code. Before, some very small errors could lead to crashes by the second lap, but with this code, we decrease the effect of those small errors substantially, since every turn will always be mostly straight, no matter at what angle it starts.

 - When the for loop ends, the breaking function (whichs stops the robot) sets in and the robot stops.

[Phase 2](src/):
Phase 2 consists of two different programs, one for the arduino nano every, and one for the nicla vision. Check each of their respective directories to see the code. Since the code is not finished and is pretty minimal right now, only the explanation for the integration can be fully developed
- The integration uses the wire library to connect the Nicla Vision and Nano every. It connects them bya I2C, or SCl and SDA connection. In both programs, the baudrate is set to 100000 and the address is set to the number 8. In the code it can be seen how the Nicla Vision is set to be the master, this means that the nicla vision will send information to the nano every and the nano every will respond to that information. If the nano every was the master, it would ask the nicla vision for information and it would use that information accordingly. 

- Currently, the Nicla Vision sends 0 if it detects green, and 1 if it detects red, when it send information, the nano every starts the recieveEvent function which processes the information and reacts accordingly

## Electromechanical Components
A lithium battery pack is being used in 3S1P configuration where 2 batteries are in parallel and then this pack is in series with the 2 batteries, to give a total of 12.6 Volts with 1900mAh capacity, together with the help of a BMS to manage the power input and output.
From this battery pack we are getting two cables, of which one has a male terminal and the other has a female terminal.

4 ultrasonic arduino sensors HC-SRO4 are being used. This sensor provides 2cm to 400cm of non-contact measurement functionality with a ranging accuracy that can reach up to 3mm. Each HC-SR04 module includes an ultrasonic transmitter, a receiver and a control circuit. There are only four pins that you need to worry about on the HC-SR04: VCC (Power), Trig (Trigger), Echo (Receive), and GND (Ground).

We used a Nicla Vision to detect the color of the obstacles, we made an algorithm to give the robot the ability to differentiate between green and red. 
We connected the Nicla Vision with the Arduino Nano Every through the SCL and SDA signals. 
When the Nicla Vision passes the information to the arduino about the color of the obstacle the arduino processes it and decides to turn left or right depending on whether the obstacle is red or green. 

Here is how the components are connected:
- [Wiring Diagrams](schemes/)

Here are some pictures of the batteries and sensors that we use: 
- [Batteries](v-photos/Electromechanical-Components/)

## Power Management
The battery power is separated in two parts. 
One part feeds the Arduino Nano Every and the Ultrasonic Sensors with 5 volts. This part make use of the Mini Power Supply HW-131 to regulate the voltage. 
The other part feeds the H-Bride TB6612 with 12 volts. 
Of course, all grounds are connected, but the 12 volts are only supplied in one positive line, this is the line that feeds the H-Bridge. 
Yet, this line connects to another positive line, but with the L7805 voltage regulator. This voltage regulator ensures that the servo and the power LED get 5 volts. Finally, the servo uses a capacitor to store energy in the case that the current demand is too high.

""Note: We use a porta fusible for safety of a short circuit in the 12 volts outing. It is highly recomended that this is used as this lithium battery could lead to a very dangerous safety hazard if not managed carefully. 

- [You can see the vehicle picture for reference](v-photos/)


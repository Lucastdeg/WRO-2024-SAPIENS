# WRO-2024-SAPIENS
WRO-2024-SAPIENS

Throughout this read me make sure to link to other parts of the github, 
For example, I mention the strategy to dodge obstacles, I link the code module that covers that

## Engineering Materials
All the materials used for our solution
- 1 Arduino Nano Every (https://docs.arduino.cc/hardware/nano-every/)
- 1 TB6612 H-Bridge (HW-166)
- 3 Ultrasonic Sensors (1 at the front, the other two right and left)
- 1 Small on/off switch
- 1 PushButton
- 2 Blue LEDS (1 for power, one for indication of the code)
- 1 Yellow LED (Indication of the code)
- 2 resistors (360 Ohms) for code Indicator LEDS
- 16 Proto Wires
- 33 Rigid Wires (Hand cut)
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

## Contents
- [Source Code](src/)
This is just a test to see if content linking works

## Introduction
Brief intro on our solution

For our robotics competition, we developed an innovative navigation solution that leverages Arduino's vision capabilities to detect and respond to color signals. By utilizing a camera module, our robot can identify green and red colors, which dictate its turning directions—left for green and right for red. This color-based navigation system is complemented by a comprehensive obstacle detection and avoidance mechanism.

To enhance our robot's awareness and maneuverability, we integrated four ultrasonic sensors—three positioned at the front and one at the back. These sensors provide real-time distance measurements, enabling the robot to detect and avoid obstacles from multiple directions. Our approach demonstrates the synergy of vision technology, ultrasonic sensing, and precise control algorithms, resulting in a responsive and reliable robotic solution.

## Strategy
Explain what strategy we used for our solution
Make sure to link code here and mention how we detected and dodged the obstacles

Strategy Explanation
Color Detection with Arduino:
We integrated a Nicla vision with the arduino nano every to accurately detect colors. This system allows the robot to recognize green and red colors, which serve as signals to turn left or right, respectively. By using a camera module and appropriate color detection algorithms, the robot can continuously monitor its environment and respond in real time.

Directional Decision Making:
Upon detecting the color green, the robot is programmed to turn left. Conversely, when red is detected, the robot turns right. This color-based decision-making process ensures that the robot navigates efficiently and avoids obstacles or follows a designated path based on the predefined color signals.

Obstacle Detection and Avoidance:
In addition to color detection, our robot is equipped with sensors to identify and dodge obstacles. When an obstacle is detected, the robot assesses the best path to avoid the obstacle, and then resumes its course, using the color signals to guide its direction. This dual approach of color-based navigation and obstacle avoidance enhances the robot’s ability to maneuver in complex environments.

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

## Mobility 
Explain how we managed the mobility
Link the chasis and models
The design for the steer
Make sure to include the problem we faced with the angle range
how it is 80-120 but 90 is stationary
Also include the speed management
the equations used for speed

## Code Explanation
Explaining different modules used and stuff
Make sure to link 

## Electromechanical Components
Power source
Sensors used
How every electro mechanical component connects
Make sure to link the wiring diagram and all the pictures of the batteries and the sensors
Also link the part of the code that covers sensors and detection
Should mention Nicla here too

A lithium battery pack is being used in 3S1P configuration where 2 batteries are in parallel and then this pack is in series with the 2 batteries, to give a total of 12.6 Volts with 1900mAh capacity, together with the help of a BMS to manage the power input and output.
From this battery pack we are getting two cables, of which one has a male terminal and the other has a female terminal.

4 ultrasonic arduino sensors HC-SRO4 are being used. This sensor provides 2cm to 400cm of non-contact measurement functionality with a ranging accuracy that can reach up to 3mm. Each HC-SR04 module includes an ultrasonic transmitter, a receiver and a control circuit. There are only four pins that you need to worry about on the HC-SR04: VCC (Power), Trig (Trigger), Echo (Receive), and GND (Ground).


- [Wiring Diagrams](schemes/)

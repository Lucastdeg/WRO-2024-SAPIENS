# WRO-2024-SAPIENS
WRO-2024-SAPIENS

Throughout this read me make sure to link to other parts of the github, 
For example, I mention the strategy to dodge obstacles, I link the code module that covers that

## Engineering Materials
All the materials used for our solution
1. Also numbered
2. Second material
3. etc...

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

## ELectromechanical Components
Power source
Sensors used
How every electro mechanical component connects
Make sure to link the wiring diagram and all the pictures of the batteries and the sensors
Also link the part of the code that covers sensors and detection
Should mention Nicla here too

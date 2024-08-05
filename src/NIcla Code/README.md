# Nicla Vision Code

The code is explained in the main readme file. 

Just for you though, Ill vaguely explain it here as well
The code consists of one main function and inside of that there are multiple functions

- cameraConfig is a higher order function that sets up the camera and takes a picture 
- calculate_average_color and get_consecutives both are used to define the obstacle color
- get_consecutives measures the consecutive pixels that are red/green
- calculate_average_color... calculated the average color
- They both go pixel by pixel 
- Send info simply sends a value, (0 or 1) to the specified address
- I recommend greatly that you look at the code, there are multiple comments that get into more detail on the integration
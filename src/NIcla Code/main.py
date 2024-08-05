# Untitled - By: lucas - Thu Jul 11 2024

import sensor, image, time
import pyb # Import module for board related functions
from pyb import I2C
import utime
import machine

def main():

    pyb.usb_mode("CDC")  # Disable USB mode to avoid waiting for a connection

    utime.sleep_ms(2000)
    #i2c = I2C(2)                         # create on bus 2
    i2c = I2C(1, I2C.MASTER)             # create and init as a master
    i2c.init(I2C.MASTER, baudrate=100000) # init as a master

    # And a short delay to wait until the I2C port has finished activating.

    redLED = pyb.LED(1) # built-in red LED
    greenLED = pyb.LED(2) # built-in green LED
    blueLED = pyb.LED(3) # built-in blue LED

    def sendInfo(value):
        # Convert the values to a byte array
        #data_to_send = bytearray([value, color])
        data_to_send = bytearray([value])

        try:
            # Send the data to the Arduino Nano Every (slave address 0x08)
            i2c.send(data_to_send, addr=0x08)
            print("Data sent:", data_to_send)
        except OSError as e:
            print("I2C Error:", e)

        time.sleep(1)  # Wait for a second before sending the next data

    def cameraConfig(formats=sensor.RGB565, resolution=sensor.QVGA):
        sensor.reset() # Initialize the camera sensor.
        sensor.set_pixformat(formats) # Sets the sensor to RGB
        sensor.set_framesize(resolution) # Sets the resolution to 320x240 px
        #https://en.wikipedia.org/wiki/Display_resolution_standards

        def takePicture(flip=False, mirror=False, frames=200, msg=""):
            sensor.set_vflip(flip)
            sensor.set_hmirror(mirror)

            #redLED.on()
            sensor.skip_frames(time=frames)
            #redLED.off()
            #blueLED.on()

            img = sensor.snapshot()

            #blueLED.off()

            return img

        return takePicture

    def get_biggest_color(red, green, blue):
        if red > green and red > blue and red < 70 and red > 20:
            return 1, 0
        elif green > red and green > blue and green < 70 and green > 20:
            return 0, 1
        else:
            return 0, 0

    def calculate_average_color(img, roi):
        x, y, w, h = roi
        r_total, g_total, b_total = 0, 0, 0
        pixel_count = w * h
        for i in range(x, x + w):
            for j in range(y, y + h):
                r, g, b = img.get_pixel(i, j)
                r_total += r
                g_total += g
                b_total += b

        r_avg = r_total // pixel_count
        g_avg = g_total // pixel_count
        b_avg = b_total // pixel_count

        return (r_avg, g_avg, b_avg)

    def get_consecutives(img):
        r_p_t, g_p_t = 0, 0
        r_pixel_count, g_pixel_count = 0, 0
        consecutive_red = 0
        consecutive_green = 0
        startr = True
        startg = True

        for i in range(1, 320):
            for j in range(1, 240):
                r, g, b = img.get_pixel(i, j)
                r_p_t, g_p_t = get_biggest_color(r, g, b)
                if r_p_t:
                    consecutive_red += 1
                elif g_p_t:
                    consecutive_green += 1
                else:
                    consecutive_red = 0
                    consecutive_green = 0

                if consecutive_red > 10:
                    if startr:
                        r_pixel_count = 11
                        startr = False
                    else:
                        r_pixel_count += 1
                if consecutive_green > 10:
                    if startg:
                        g_pixel_count = 11
                        startg = False
                    else:
                        g_pixel_count += 1

        return r_pixel_count, g_pixel_count

    while(True):
        TakePic = cameraConfig()
        img = TakePic()
        r, g, b = calculate_average_color(img, (1, 1, 319, 239))
        #con_red, con_green = get_consecutives(img)
        #print(get_consecutives(img))
        #print(r, g, b)

        if r > g:
            print("RED")
            greenLED.off()
            redLED.on()
            sendInfo(1)
        elif g > r:
            print("GREEN")
            redLED.off()
            greenLED.on()
            sendInfo(0)
        else:
            print("NEITHER")

if __name__ == "__main__":
    main()


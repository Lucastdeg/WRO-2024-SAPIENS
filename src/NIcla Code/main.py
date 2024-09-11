import sensor, image, time
import pyb  # Import module for board-related functions
from pyb import I2C
import utime

def main():
    pyb.usb_mode("CDC")  # Disable USB mode to avoid waiting for a connection
    utime.sleep_ms(2000)

    # Initialize I2C in slave mode on bus 1 with address 0x08
    i2c = I2C(1, I2C.SLAVE, addr=0x08, baudrate=100000)

    # LEDs for status indication
    redLED = pyb.LED(1)  # built-in red LED
    greenLED = pyb.LED(2)  # built-in green LED
    blueLED = pyb.LED(3)

    def sendInfo(value):
        data_to_send = bytearray([value])
        try:
            i2c.send(data_to_send)  # Send the data to the master
            print("Data sent:", data_to_send)
        except OSError as e:
            print("I2C Error:", e)
            redLED.on()  # Indicate error with red LED
            time.sleep(0.5)
            redLED.off()

        time.sleep(1)  # Wait for a second before the next operation

    def cameraConfig(formats=sensor.RGB565, resolution=sensor.QVGA):
        sensor.reset()  # Initialize the camera sensor.
        sensor.set_pixformat(formats)  # Sets the sensor to RGB
        sensor.set_framesize(resolution)  # Sets the resolution to 320x240 px
        sensor.set_brightness(3)

        def takePicture(flip=False, mirror=False, frames=200):
            sensor.set_vflip(flip)
            sensor.set_hmirror(mirror)
            sensor.skip_frames(time=frames)
            img = sensor.snapshot()
            return img

        return takePicture

    def find_color_blobs(img):
        # Define color thresholds for broader sensitivity
        red_threshold = (10, 100, 10, 127, 0, 127)  # Expanded red range
        green_threshold = (15, 100, -80, -8, -60, 60)  # Expanded green range

        red_blobs = img.find_blobs([red_threshold], pixels_threshold=18, area_threshold=18, merge=True)
        green_blobs = img.find_blobs([green_threshold], pixels_threshold=18, area_threshold=18, merge=True)

        red_pixel_count = sum(blob.pixels() for blob in red_blobs)
        green_pixel_count = sum(blob.pixels() for blob in green_blobs)

        print("Red pixels:", red_pixel_count, "Green pixels:", green_pixel_count)

        if red_pixel_count > green_pixel_count and red_pixel_count > 100:
            return 1  # Red detected
        elif green_pixel_count > red_pixel_count and green_pixel_count > 100:
            return 0  # Green detected
        else:
            return -1  # No significant blobs detected

    blueLED.on()  # Turn on red LED to indicate the device is ready

    while True:
        TakePic = cameraConfig()
        img = TakePic()

        color_detected = find_color_blobs(img)

        if color_detected == 1:  # Red detected
            print("RED detected")
            greenLED.off()
            redLED.on()
            sendInfo(1)
        elif color_detected == 0:  # Green detected
            print("GREEN detected")
            redLED.off()
            greenLED.on()
            sendInfo(0)
        else:  # Neither detected
            print("NEITHER detected")
            redLED.off()
            greenLED.off()
            blueLED.on()
            sendInfo(255)  # Send 255 as a signal for "neither"

        time.sleep(1)

if __name__ == "__main__":
    main()



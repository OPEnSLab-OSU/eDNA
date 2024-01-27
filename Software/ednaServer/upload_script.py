import time

# This upload script helps with reconnecting the serial port after reupload.
# Sometimes it takes a couple of seconds before the computer recognizes the serial
# port.

# pylint: disable=undefined-variable
Import("env")


def after_upload(source, target, env):
    seconds = 3
    print("Delay after upload for " + str(seconds) + " seconds...")
    time.sleep(seconds)
    print("Done!")


# pylint: disable=undefined-variable
env.AddPostAction("upload", after_upload)

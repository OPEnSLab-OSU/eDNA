import time

# pylint: disable=undefined-variable
Import("env")


# Need to wait for initialization of Serial
def after_upload(source, target, env):
    seconds = 2
    print("Delay after upload " + str(seconds) + " forseconds...")
    time.sleep(seconds)
    print("Done!")


# pylint: disable=undefined-variable
env.AddPostAction("upload", after_upload)

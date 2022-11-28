###################################################
# LIBRARY SOURCES
###################################################

# Set mylib folder path.
# IMPORTANT NOTE: change MYLIBTEMPLATE_PATH to your Driver name e.g. JOYSTICK_MYLIBPATH
# e.g. JOYSTICK_MYLIBPATH=/holme/csse3010/mylib/joystick
PWM_PATH=/home/csse3010/csse3010/mylib/pwm
PWM_CONFIG_PATH = /home/csse3010/csse3010/stages/stage2

# Set folder path with header files to include.
CFLAGS += -I$(PWM_PATH) -I$(PWM_CONFIG_PATH)

# List all c files locations that must be included (use space as separate e.g. LIBSRCS += path_to/file1.c path_to/file2.c)
LIBSRCS += $(PWM_PATH)/s4433912_hal_pwm.c


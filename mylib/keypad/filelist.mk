###################################################
# LIBRARY SOURCES
###################################################

# Set mylib folder path.
# IMPORTANT NOTE: change MYLIBTEMPLATE_PATH to your Driver name e.g. JOYSTICK_MYLIBPATH
# e.g. JOYSTICK_MYLIBPATH=/holme/csse3010/mylib/joystick
KEYPAD_PATH=/home/csse3010/csse3010/mylib/keypad
# KEYPAD_CONFIG_PATH = /home/csse3010/csse3010/stages/stage3
KEYPAD_CONFIG_PATH = /home/csse3010/csse3010/stages/project

# Set folder path with header files to include.
CFLAGS += -I$(KEYPAD_PATH) -I$(KEYPAD_CONFIG_PATH)

# List all c files locations that must be included (use space as separate e.g. LIBSRCS += path_to/file1.c path_to/file2.c)
LIBSRCS += $(KEYPAD_PATH)/s4433912_hal_keypad.c
LIBSRCS += $(KEYPAD_PATH)/s4433912_os_keypad.c

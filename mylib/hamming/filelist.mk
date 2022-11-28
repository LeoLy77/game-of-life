###################################################
# LIBRARY SOURCES
###################################################

# Set mylib folder path.
# IMPORTANT NOTE: change MYLIBTEMPLATE_PATH to your Driver name e.g. JOYSTICK_MYLIBPATH
# e.g. JOYSTICK_MYLIBPATH=/holme/csse3010/mylib/joystick
HAMMING_PATH=/home/csse3010/csse3010/mylib/hamming
# KEYPAD_CONFIG_PATH = /home/csse3010/csse3010/stages/stage3

# Set folder path with header files to include.
CFLAGS += -I$(HAMMING_PATH)

# List all c files locations that must be included (use space as separate e.g. LIBSRCS += path_to/file1.c path_to/file2.c)
LIBSRCS += $(HAMMING_PATH)/s4433912_lib_hamming.c


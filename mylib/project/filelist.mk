###################################################
# LIBRARY SOURCES
###################################################

# Set mylib folder path.
# IMPORTANT NOTE: change MYLIBTEMPLATE_PATH to your Driver name e.g. JOYSTICK_MYLIBPATH
# e.g. JOYSTICK_MYLIBPATH=/holme/csse3010/mylib/joystick
CAG_PATH = /home/csse3010/csse3010/mylib/project

# Set folder path with header files to include.
CFLAGS += -I$(CAG_PATH) 

# List all c files locations that must be included (use space as separate e.g. LIBSRCS += path_to/file1.c path_to/file2.c)
LIBSRCS += $(CAG_PATH)/s4433912_CAG_keypad_grid.c
LIBSRCS += $(CAG_PATH)/s4433912_CAG_keypad_mnemonic.c
LIBSRCS += $(CAG_PATH)/s4433912_CAG_Simulator.c
LIBSRCS += $(CAG_PATH)/s4433912_CAG_share.c
LIBSRCS += $(CAG_PATH)/s4433912_CAG_Display.c
LIBSRCS += $(CAG_PATH)/s4433912_CAG_joystick.c

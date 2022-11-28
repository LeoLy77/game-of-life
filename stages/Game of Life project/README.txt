
## Usage
Display
- enable GRID_LABEL in CAG_share to see numerical x and y axis labels. Helpful to locate subgrid and position of cells
- the cursor is hidden for observation convenience

Keypad grid mode
- Key press should register and show on display instantly regardless if the simulation is running or not
- Key press is not registered or shown on display if the simulation is deinitialised
- Unspecified keys (E and D) will not be registered by the simulation when pressed

Keypad mnemonic mode
- this mode was designed to function as closely to the alphanumeric phone keypad as possible
- characters are all in uppercase 
- press multiple times under 500ms the same key will iterate through the characters on the key starting with the number (eg: 7->P->Q->R->S)
- release the key more than 500ms to register the desired character
- enable DEBUG in CAG_share to see registered characters printed out underneath the grid

Joystick
- my Joystick is very sensitive where it goes from mid-point to max in the slightest switch, my joystick is fully functional in both changing colour and update time,
so please be patient and gentle with the joystick to see the desired implemented results, if you have the same joystick

Code style
- I notice sometimes the uploaded code on git appears systematically different in terms of spaces, indentations and tabs,
this is a known issue commonly happens between VSCode and Git. So please let me know if my code does not look abide to the code style

Possible hardware issues you might encounter
- please reset Putty if the display appears to freeze up. This problem has never occured with screen (kermusb)

'''
Created on Jan 26, 2013

Global Variables for the control logic
-   All variables set by main in the control logic are stored in the
    global variable class.  Global variables can be accessed from other
    classes, however, they should not be set outside of main and the GUI
    Handler.

'''

    

instructions = None
# Array for all current data from arduino
#    Format: TODO
currentData = []
functionQueue = []
queueParameters = []
boundaries = []
run = True
auto = False
currentProcess = None
currentParams = None
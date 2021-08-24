# Window-Manager
## Functionality
This is a window manager that works based on hotkey input. It uses the Win32 API to find and control window handles. In the configuration files that are presents I have it set so that the first hotkey brings up two file managers in split window and the second and third ones bring up chrome windows. [**Link to youtube demo.**](https://youtu.be/kFFMxPCZ-r8)

### Controls
The controls are CTRL+ALT+1, CTRL+ALT+2,CTRL+ALT+3, and CTRL+ALT+4 for different window setups **If CTRL+ALT does not switch the applications displayed then try ALT**. 1 and 2 have file explorer windows while 3 and 4 are set of chrome windows. 
To launch each window set up you would do CTRL+SHFT+1 or some other number. After launching a set of windows, update them into the application by pressing ALT+0.
To clear all windows associated with the app press ALT+`. **Additional documentation on how to change hotkeys and setups coming soon**.

## Branches 
* master -- Has the app run with a console window. Define DEBUG if you want to see some print statements tracing the code.
* Backup branches -- Not to be used.
* Window-Manager -- The master branch but running without a console window. 
For all branches run the app only in **Release x64**.


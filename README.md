# Installation

## Windows
- Download the .scr file from the latest release.
- Move the SCR file to your main Windows directory. For example, in Windows 10/11, move your file to the C:\Windows\System32 directory.
- In the Windows search bar, type "screen saver." Select the Change screen saver option from the search results.
- The Screen Saver Settings window appears. From the Screen saver dropdown menu, select 1337. Customize the remainder of your screen saver settings, and then click Apply.

## MacOS
- Soon

## GNU/Linux
##### For Debian based distributions

- Run the following commands.

```shell
$ sudo apt -y install xscreensaver
$ wget -q https://github.com/akanmuratcimen/1337-screensaver/releases/download/v1.0.0/1337
$ sudo chmod +x 1337
$ sudo mv 1337 /usr/libexec/xscreensaver/
```

- Open and close XScreenSaver application to generate .xscreensaver file in the user's directory.
- Open .xcscreensaver file with a text editor and add "- 1337 -root \n\\" to the end of the list of screensavers.
- Reopen XScreenSaver application.
- Select 'Only One Screen Saver' mode.
- Select 1337 from the screen savers list.
- Set the 'Cycle After' value to 0.


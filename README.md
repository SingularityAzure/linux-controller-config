# linux-controller-config
A GUI tool for remapping controller inputs on Linux for those pesky times your controller doesn't work out of the box.

Using wxWidgets and the linux/joystick API, this provides a nice-to-use interface for remapping controls.

# Planned Features
These aren't here yet but they're next on the list.
- Saving/loading maps in a file (preferably automatically).
- Axis calibration.


# Installation
### First you're going to need wxWidgets' development package.
On Debian-based systems (Like Ubuntu), you can get it like so:

for GTK3 (Preferred)
```
sudo apt install libwxgtk3.0-gtk3-dev
```
or GTK2 (But not both)
```
sudo apt install libwxgtk3.0-dev
```

### Then you'll want to clone this repository.
Open a terminal and navigate to where you put git repositories.

Ex: `cd ~/Downloads` or `cd ~/Git`

Then clone it and navigate into the directory it created.
```
git clone git@github.com:SingularityAzure/linux-controller-config.git
cd linux-controller-config
```
Then calling make will build it.
```
make -j2
```
And you can also install it like so:
```
sudo make install
```
### Now you should be able to run it.
Either by calling controller-config from a terminal or by searching for it in your installed apps.

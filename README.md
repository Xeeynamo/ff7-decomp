# Final Fantasy VII decomp (PS1)

## Contribute

This project is open to contributors!
Decompiling a video game is a collaborative effort made possible by passionate people.

* If you’d like to know more about how to contribute, feel free to reach out to me on Discord.
* New pull requests are always welcome!

## Build

Clone the repository:

```shell
git clone git@github.com:Xeeynamo/ff7-decomp.git --recursive &&\
cd ff7-decomp
```

Install the necessary dependencies:

```shell
make requirements

# Debian/Ubuntu
sudo add-apt-repository ppa:longsleep/golang-backports
sudo apt update
sudo apt install golang-go ninja-build 7zip bchunk binutils-mipsel-linux-gnu gcc-mipsel-linux-gnu

# Arch Linux
sudo pacman -S go ninja 7zip bchunk
yay mipsel-linux-gnu-binutils mipsel-linux-gnu-gcc
```

Place the required disk images:

```shell
'disks/Final Fantasy VII (USA) (Disc 1).bin'
'disks/Final Fantasy VII (USA) (Disc 1).cue'
'disks/Final Fantasy VII (USA) (Interactive Sampler CD).bin' # OPTIONAL!
'disks/Final Fantasy VII (USA) (Interactive Sampler CD).cue' # OPTIONAL!
```

Then build the project:

```shell
make
```

## Game strings

The game uses a custom character encoding instead of standard ASCII.
In this codebase, such strings are represented as `_S("Hello FF7!")`.

To decode a FF7-style string into readable ASCII, use the included tool:

```shell
# compile it at least once
make bin/str

# specify an overlay and an offset in hex
bin/str disks/us/MENU/SAVEMENU.MNU 12DF8

# The program will print the decoded string to your terminal:
Saving. Do not remove Memory card.
```

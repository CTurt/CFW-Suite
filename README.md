# CFW Suite
A collection of tools which can be used to modify some of the text from a DS or DS Lite firmware, to create a custom firmware (CFW).

See [this video](https://www.youtube.com/watch?v=bF7NuRreoCU) for an example.

Code from Chism, Loopy, Lick, and others has been used.

## Steps to modify the "Touch the Touch Screen to continue." text
### Dump original firmware
- (Optional but recommended) install a version of [FlashMe](https://home.comcast.net/~olimar/flashme/), this adds a failsafe to your firmware so you can recover if a future flash fails,
- Dump your DS firmware using [DSBF Dump](http://www.ds-scene.net/?s=viewtopic&nid=2460),

### Modifying the firmware
- Compile guiTool by running `make`,
- Extract gui data from your firmware with `guiTool firmware.bin -e gui.bin`,
- Make any changes you want to the `gui.bin` file (note: text is Unicode), the "Touch the Touch Screen to continue." text starts at `0x2c20` in the file.
- Make a copy of your firmware (here it is called `cfw.bin`),
- Inject gui data into your custom firmware with `guiTool cfw.bin -i gui.bin`,
- (Optional but recommended) run [DeSmuME](http://desmume.org/), go to `Config->Emulation Settings` and configure it to boot from your custom firmware; this is just to test that it works,

### Flashing to hardware (fwManager - recommended method)
- Compile fwManager by running `make`, or download a prebuilt binary, copy it to your flashcard,
- Make a new directory called `firmwares` on your flashcard,
- Place your modified firmware (`cfw.bin`) here,
- Run fwManager and select your custom firmware; when the installer is running, short the SL1 terminal as if you were installing FlashMe.

### Flashing to hardware (FlashMeInjector - old method for homebrew loaders that don't support libfat)
- Compile FlashMeInjector by running `make`,
- Inject your firmware into the FlashMe installer with `FlashMeInjector flashme.nds -lite cfw.bin` if you are using a DS Lite, or `FlashMeInjector flashme.nds -phat cfw.bin` if you are using an original DS,
- Run your modified FlashMe installer on your DS,
- FlashMe firmwares have an additional checksum, which will be incorrect, FlashMe will tell you the correct one and won't flash anything,
- Correct the FlashMe checksum with the one your DS told you, and inject the new firmware back into `flashme.nds`,
- Run `flashme.nds` again, from here the installation process will be the same as if you were installing FlashMe normally (SL1 must be shorted),

If you follow the optional steps, then the risk of a brick will be relatively low; however, I take no responsibility if a brick does happen as a result of using these tools.

## Uninstalling
Assuming you kept a backup of your original firmware, you can flash it with fwManager. Alternatively you may use noflashme.

### No content owned by Nintendo is contained in this repo!
No firmwares are here, you must dump your own. The firmwares contained in the FlashMe installer have been removed, you must inject your own if you are using FlashMeInjector.
# EarthLiveCPP

A CPP rewrite of [EarthLiveShell](https://github.com/cthbleachbit/EarthLiveShell.git) which is my shell script rewrite of [EarthLiveSharp](https://github.com/bitdust/EarthLiveSharp) which is written in C# and windows-only.

#### Contributors

* Yue Sun <yues@cs.wisc.edu>
* Tianhao Chai <tianhao@cs.wisc.edu>

### Rationale

The C# one can only run on windows, the shell implementation has performance constraints. For original purpose, check [EarthLiveShell](https://github.com/cthbleachbit/EarthLiveShell.git).

### Feature list

###### Control part:

Program invoked:

* Obtain settings from configuration files.
* Check if it is auto started i.e. start on boot / login.
* if yes, directly __invoke bachground process__.
* if no, __show control interface__, when user can start / stop / change settings.

When invoking bachground process:

* Save user's original wallpaper settings if current wallpaper is not our dear planet Earth.
* Set wallpaper
* daemonize bachground process with parameters of refresh interval and image resolution ratio.

When user clicked stop:

* restore user's original wallpaper
* kill background process

When user changed a settings:

* writes new settings to configuration file
* restart background process

###### Background process part:

1. Obtain refresh interval and resolution ratio from command line.
2. Check if there is a update released.
3. if no, go to 7.
4. if yes, download image tiles
5. if there is multiple tiles, stitch them together with `imagemagick`.
6. update the image.
7. wait until next refresh interval.

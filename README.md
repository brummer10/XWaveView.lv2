# XWaveView.lv2

simple scope LV2 plugin

RMS                        |  Direct
:-------------------------:|:-------------------------:
![XWaveView](https://github.com/brummer10/XWaveView.lv2/raw/master/XWaveView.png) | ![XWaveView](https://github.com/brummer10/XWaveView.lv2/raw/master/XWaveView_direct.png)


## Features

- Display RMS waveform
- Display direct waveform


## Dependencys

- libcairo2-dev
- libx11-dev

## Build
- git submodule init
- git submodule update
- make
- make install # will install into ~/.lv2 ... AND/OR....
- sudo make install # will install into /usr/lib/lv2

that's it.

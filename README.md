# ARticated
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](./LICENSE)
Master: [![Build Status](https://travis-ci.org/derpicated/articated.svg?branch=master)](https://travis-ci.org/derpicated/articated)
Devel: [![Build Status](https://travis-ci.org/derpicated/articated.svg?branch=devel)](https://travis-ci.org/derpicated/articated)

An augmented reality application.

![ARticated](./images/application/articated_application.jpg)

This augmented reality application uses ONLY camera images and some fancy math to track markers
across 3D space. It then draws virtual 3D objects as if they were part of reality.

## Developers

This application was developed as an educational project for two
Embedded Systems Engineering students, for their minor in Embedded Vision.
All vision processing, 2D to 3D transformation maths,
and rendering code has been written from scratch by:

-   [Ingmar Delsink](https://github.com/idelsink)
-   [Menno van der Graaf](https://github.com/Mercotui)

## Usage

Open the application, whether it be on desktop or an android phone,
and point the camera so that at least 3 markers are clearly in view.
Press the largest button to set the current position as reference.
Now you can move the camera around, keeping at least 3 markers in view.

## Markers

The application needs to track at least 3 unique markers to calculate the camera movement, although more can be used.

These markers are loosely defined groups of black dots on a white background.
The markers are identified by their dot count, so these have to be unique.
A minimum of 2 and a maximum of 9 dots are required for a marker to be valid.

See below, the markers can be drawn by hand. This configuration features marker #3, #4, and #5. Make sure to leave plenty of white-space between each marker, so they don't merge together.

![Markers](./images/drawn_markers.jpg)

## Dependencies

### Desktop

-   Qt 5.12
-   OpenGL 4.1

#### Fedora

Install the following libraries on Fedora:

```sh
dnf install \
  qt5-qtbase-devel \
  qt5-qtbase-gui \
  qt5-qtquickcontrols2-devel \
  qt5-qtmultimedia-devel

```

### Android

-   Qt 5.12
-   OpenGL-ES 3.0
-   Android SDK + NDK

## Tested

Tested on:

-   Ubuntu 14.04.3 LTS ([See Travis-ci](https://travis-ci.org/derpicated/articated))
-   Fedora 29
-   Android 8.0.1

## Building

### Desktop

When building this application for desktop, pass the following CMake variables:

| Variable           | Type | Description                                                                          |
|:-------------------|:-----|:-------------------------------------------------------------------------------------|
| Qt5_DIR (Optional) | PATH | Path to the Qt CMake directory. Only necessary when Qt is not installed system wide. |

#### Example

```sh
mkdir build && cd build

cmake .. -DQt5_DIR=/home/user/Qt/5.7/gcc_64/lib/cmake/Qt5

make
```

### Android

When building this application for Android, pass the following CMake variables:

| Variable            | Type   | Description                     |
|:--------------------|:-------|:--------------------------------|
| Qt5_DIR             | PATH   | Path to the Qt CMake directory. |
| QT_ANDROID_SDK_ROOT | PATH   | Path to the Android SDK root    |
| ANDROID_NDK         | PATH   | Path to the Android NDK root    |
| JAVA_HOME           | PATH   | Path to the Java root           |
| ANDROID             | ON/OFF | Build for Android               |

#### Example

```sh
mkdir build && cd build

cmake .. \
-DCMAKE_TOOLCHAIN_FILE=/home/user/Android/Sdk/ndk-bundle/build/cmake/android.toolchain.cmake \
-DANDROID=ON \
-DQt5_DIR=/home/user/Qt/5.12.1/android_armv7/lib/cmake/Qt5 \
-DQT_ANDROID_SDK_ROOT=/home/user/Android/Sdk \
-DANDROID_NDK=/home/user/Android/Sdk/ndk-bundle \
-DJAVA_HOME=/usr/lib/jvm/java-1.8.0-openjdk-1.8.0.191.b13-0.fc29.x86_64

make
```

### Extra note

When switching targets, make sure to delete the old build files. So an
`rm -rf ./` on the build directory would suffice. Or for convenience, two build
folders can be made, one for android and one for desktop. This way switching to
a different target is a mere switching of directories.

## License

> You can check out the full license [here](./LICENSE)

This project is licensed under the terms of the **MIT** license.

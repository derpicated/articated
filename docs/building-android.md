# Building For Android

Building for android is currently untested and potentially problematic.

## Dependencies

- Qt 5.14
- OpenGL-ES 3.0
- Android SDK + NDK

## Building

When building this application for Android, pass the following CMake variables:

| Variable            | Type   | Description                     |
|:--------------------|:-------|:--------------------------------|
| Qt5_DIR             | PATH   | Path to the Qt CMake directory. |
| QT_ANDROID_SDK_ROOT | PATH   | Path to the Android SDK root    |
| ANDROID_NDK         | PATH   | Path to the Android NDK root    |
| JAVA_HOME           | PATH   | Path to the Java root           |
| ANDROID             | ON/OFF | Build for Android               |

### Example

```sh
mkdir build-android && cd build-android

cmake .. \
-DCMAKE_TOOLCHAIN_FILE=Sdk/ndk-bundle/build/cmake/android.toolchain.cmake \
-DANDROID=ON \
-DQt5_DIR=/home/user/Qt/5.14.1/android_armv7/lib/cmake/Qt5 \
-DQT_ANDROID_SDK_ROOT=Sdk \
-DANDROID_NDK=Sdk/ndk-bundle \
-DJAVA_HOME=/usr/lib/jvm/java-1.8.0-openjdk

make
```

## Extra note

When switching targets, make sure to delete the old build files. So an
`rm -rf ./` on the build directory would suffice. Or for convenience, two build
folders can be made, one for android and one for desktop. This way switching to
a different target is a mere switching of directories.

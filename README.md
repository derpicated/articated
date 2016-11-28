# ARticated Embedded

An augmented reality project.
Work in progress.

Pass the following parameters:

-   Qt5_DIR
-   QT_ANDROID_SDK_ROOT
-   QT_ANDROID_ANT
-   ANDROID_NDK
-   JAVA_HOME

For example:

```sh
cmake .. \
-DQt5_DIR=/home/user/Qt/5.7/android_armv7/lib/cmake/Qt5 \
-DQT_ANDROID_SDK_ROOT=/home/user/.local/android-sdk-linux \
-DQT_ANDROID_ANT=/usr/bin/ant \
-DANDROID_NDK=/home/user/.local/android-ndk-r13b \
-DJAVA_HOME=/usr/lib/jvm/java-1.8.0-openjdk-1.8.0.111-3.b16.fc24.x86_64
```

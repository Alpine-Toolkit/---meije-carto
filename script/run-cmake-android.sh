# -DANDROID_ABI="armeabi-v7a with NEON"

cmake \
  -DCMAKE_TOOLCHAIN_FILE=android.toolchain.cmake \
  -DANDROID_NDK=/opt/android-ndk-r10e \
  -DCMAKE_BUILD_TYPE=Release \
  -DANDROID_ABI="armeabi-v7a" \
  $HOME/home/developpement/qtcarto-application/alpine-toolkit/qtcarto

cmake --build

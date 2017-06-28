####################################################################################################

# -DANDROID_ABI="armeabi-v7a with NEON"

#BUILD_TYPE=Release
BUILD_TYPE=Debug

rm -f android.toolchain.cmake
ln -sf /home/fabrice/home/developpement/qtcarto-application/android-build/android-cmake/android.toolchain.cmake

cmake \
  -DCMAKE_TOOLCHAIN_FILE=android.toolchain.cmake \
  -DANDROID_NDK=/opt/android-ndk-r10e \
  -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
  -DANDROID_ABI="armeabi-v7a" \
  $HOME/home/developpement/qtcarto-application/alpine-toolkit/qtcarto

cmake --build .

####################################################################################################
#
# End
#
####################################################################################################

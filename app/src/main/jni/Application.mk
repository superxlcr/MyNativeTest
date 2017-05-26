# This is the application makefile

# APP_OPTIM two options: debug, release
APP_OPTIM := release

# APP_ABI four options: armeabi, armeabi-v7a, mips, x86, all 
APP_ABI := armeabi armeabi-v7a x86

# APP_PLATFORM: used to specify the include folder
APP_PLATFORM := android-18

APP_STL:=stlport_static
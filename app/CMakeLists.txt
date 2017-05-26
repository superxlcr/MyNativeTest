cmake_minimum_required(VERSION 3.4.1)

# useless
add_library(libcommon STATIC src/main/jni/common/common.c)
add_executable(daemon src/main/jni/daemon/daemon.c)
add_executable(serviceDaemon src/main/jni/daemon/serviceDaemon.cpp)

# useful
add_library(daemon-lib SHARED src/main/jni/so/daemonLib.cpp)
target_link_libraries(daemon-lib android log)
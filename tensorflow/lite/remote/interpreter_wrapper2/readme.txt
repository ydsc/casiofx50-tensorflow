TI Cloud TFLite 2.8

Examples on how to build TIIE client pip wheel and server (EVM) plugin

Client wheel - for AWS docker container
TIIE_DIR=/home/user/ti/GIT_cloud_build_ta/cloud_build_ta/test/ti-inference-engine ./tensorflow/lite/tools/pip_package/build_remote_pip_package_with_cmake.sh native



Server plugin
TFLITE Cross compilation
     #cross compilation for arm's cloud plugin
     mkdir tflite_build_remote
     cd tflite_build_remote
     TIIE_DIR=/home/user/ti/GIT_cloud_build_ta/cloud_build_ta/test/ti-inference-engine 
     ARMCC_PREFIX=/home/user/tools/gcc-arm-9.2-2019.12-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-    
     ARMCC_FLAGS="-funsafe-math-optimizations"
     cmake -DCMAKE_C_COMPILER=${ARMCC_PREFIX}gcc \
     -DCMAKE_CXX_COMPILER=${ARMCC_PREFIX}g++ \
     -DCMAKE_C_FLAGS="${ARMCC_FLAGS}" \
     -DCMAKE_CXX_FLAGS="${ARMCC_FLAGS}" \
     -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON \
     -DCMAKE_SYSTEM_NAME=Linux \
     -DTFLITE_ENABLE_XNNPACK=ON \
     -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
     -DBUILD_SHARED_LIBS=OFF \
     -DTIIE_REMOTE_PLUGIN=ON \
     -DTIIE_PATH=${TIIE_DIR} \
     ../tensorflow/lite/

     cmake --build .


ARM crosscompilation lib
TFLITE Cross compilation
     #cross compilation for arm
     mkdir tflite_build_arm
     cd tflite_build_arm
     ARMCC_PREFIX=$ARM64_GCC_PATH/bin/aarch64-none-linux-gnu-
     ARMCC_FLAGS="-funsafe-math-optimizations"
     cmake -DCMAKE_C_COMPILER=${ARMCC_PREFIX}gcc \
     -DCMAKE_CXX_COMPILER=${ARMCC_PREFIX}g++ \
     -DCMAKE_C_FLAGS="${ARMCC_FLAGS}" \
     -DCMAKE_CXX_FLAGS="${ARMCC_FLAGS}" \
     -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON \
     -DCMAKE_SYSTEM_NAME=Linux \
     -DTFLITE_ENABLE_XNNPACK=ON \
     -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
     -DBUILD_SHARED_LIBS=ON \
     ../tensorflow/lite/

     cmake --build .

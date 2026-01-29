cmake_minimum_required(VERSION 3.10)
project(stream_analyzing)

# C++标准
set(CMAKE_CXX_STANDARD 11)              # 使用C++11标准
set(CMAKE_CXX_STANDARD_REQUIRED ON)     # 强制编译器必须使用规定标准，否则产生报错

# set(OPENCV_ROOT     "C:/myfiles/3rdparty/opencv/opencv-4.9.0-msvc/opencv-4.9.0-build/install")
# set(OPENCV_ROOT     "/opt/opencv/opencv-4.9.0/build")
# set(OPENCV_ROOT     "/usr/local/opencv490")
# set(TENSORRT_ROOT     "/opt/TensorRt/TensorRT-8.6.1.6") # pc
# set(CUDA_ROOT "/usr/local/cuda") # pc
# set(TENSORRT_ROOT     "/opt/tensorrt/TensorRT-8.6.1.6") # work pc
# set(CUDA_ROOT "/usr/local/cuda-11.7") # work pc

set(OPENCV_ROOT "" CACHE PATH "Path to OpenCV root directory")
set(TENSORRT_ROOT "" CACHE PATH "Path to TensorRT root directory")
set(CUDA_ROOT "" CACHE PATH "Path to CUDA root directory")

# 打印OPENCV_ROOT, TENSORRT_ROOT, CUDA_ROOT路径
message(STATUS "OPENCV_ROOT: " ${OPENCV_ROOT})
message(STATUS "TENSORRT_ROOT: " ${TENSORRT_ROOT})
message(STATUS "CUDA_ROOT: " ${CUDA_ROOT})

# include_directories(
#     ${PROJECT_SOURCE_DIR}/include
#     ${TENSORRT_ROOT}/include
#     ${OPENCV_ROOT}/include
#     ${CUDA_ROOT}/include
# )

# find_package(OpenCV REQUIRED)

file(GLOB_RECURSE SRC_FILES
    ${PROJECT_SOURCE_DIR}/src/*.cpp
)
# 从 SRC_FILES 中移除 main 文件
list(REMOVE_ITEM SRC_FILES
    ${PROJECT_SOURCE_DIR}/src/main.cpp
    ${PROJECT_SOURCE_DIR}/src/main_coupling.cpp
)
message(STATUS "find PROJECT_SOURCE_DIR: " ${SRC_FILES})

# set(MAIN_FILE ${PROJECT_SOURCE_DIR}/src/main.cpp)
# message(STATUS "find MAIN_FILE: " ${MAIN_FILE})

link_directories(
    ${OPENCV_ROOT}/lib
    ${TENSORRT_ROOT}/lib
    ${CUDA_ROOT}/lib64
)

add_executable(stream_analyzing ${PROJECT_SOURCE_DIR}/src/main.cpp  ${SRC_FILES})

target_include_directories(stream_analyzing PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/3rdparty
    ${OPENCV_ROOT}/include/opencv4
    ${TENSORRT_ROOT}/include
    ${CUDA_ROOT}/include
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(stream_analyzing 
    opencv_world
    nvinfer
    cudart
    pthread
)

add_executable(stream_analyzing_coupling ${PROJECT_SOURCE_DIR}/src/main_coupling.cpp 
    ${PROJECT_SOURCE_DIR}/src/Model.cpp ${PROJECT_SOURCE_DIR}/src/TrtModel.cpp
    ${PROJECT_SOURCE_DIR}/src/Assignment.cpp)

target_include_directories(stream_analyzing_coupling PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/3rdparty
    ${TENSORRT_ROOT}/include
    ${CUDA_ROOT}/include
    ${OPENCV_ROOT}/include/opencv4
)

target_link_libraries(stream_analyzing_coupling 
    opencv_world
    nvinfer
    cudart
    pthread
)

# 设置rpath
set(CMAKE_BUILD_RPATH
    ${TENSORRT_ROOT}/lib
    ${CUDA_ROOT}/lib64
    ${OPENCV_ROOT}/lib
)

set(CMAKE_INSTALL_RPATH
    ${TENSORRT_ROOT}/lib
    ${CUDA_ROOT}/lib64
    ${OPENCV_ROOT}/lib
)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE) #


install(TARGETS stream_analyzing stream_analyzing_coupling
    RUNTIME DESTINATION media_stream_analyzing/bin) # q: RUNTIME的作用是什么, a: 

install(FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/../res/config_test_images_dir.json
    ${CMAKE_CURRENT_SOURCE_DIR}/../res/config_test_speed.json
    ${CMAKE_CURRENT_SOURCE_DIR}/../res/test.mp4
    ${CMAKE_CURRENT_SOURCE_DIR}/../res/resnet50_pretrained.engine
    ${CMAKE_CURRENT_SOURCE_DIR}/../res/mobilenet_v2_pretrained.engine
    ${CMAKE_CURRENT_SOURCE_DIR}/../res/imagenet_classes.txt
    DESTINATION media_stream_analyzing/res
)

install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/../res/test_images_dir # 将整个文件夹安装过去
    DESTINATION media_stream_analyzing/res
)

install(FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/README.md
    DESTINATION media_stream_analyzing
)
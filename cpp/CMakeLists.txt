cmake_minimum_required(VERSION 3.10)
project(stream_analyzing)

# C++标准
set(CMAKE_CXX_STANDARD 11)              # 使用C++11标准
set(CMAKE_CXX_STANDARD_REQUIRED ON)     # 强制编译器必须使用规定标准，否则产生报错

# set(OPENCV_ROOT     "C:/myfiles/3rdparty/opencv/opencv-4.9.0-msvc/opencv-4.9.0-build/install")
set(OPENCV_ROOT     "/opt/opencv/opencv-4.9.0/build")
set(TENSORRT_ROOT     "/opt/TensorRt/TensorRT-8.6.1.6")
set(CUDA_ROOT "/usr/local/cuda")

# include_directories(
#     ${PROJECT_SOURCE_DIR}/include
#     ${TENSORRT_ROOT}/include
#     ${OPENCV_ROOT}/include
#     ${CUDA_ROOT}/include
# )

find_package(OpenCV REQUIRED)

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
    ${TENSORRT_ROOT}/include
    ${CUDA_ROOT}/include
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(stream_analyzing 
    ${OpenCV_LIBS}
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
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(stream_analyzing_coupling 
    ${OpenCV_LIBS}
    nvinfer
    cudart
    pthread
)

# 设置rpath
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_BUILD_RPATH
    ${TENSORRT_ROOT}/lib
    ${CUDA_ROOT}/lib64
)

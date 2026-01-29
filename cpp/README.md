
cmake .. -DOPENCV_ROOT=/usr/local/opencv490/ -DTENSORRT_ROOT=/opt/tensorrt/TensorRT-8.6.1.6/ -DCUDA_ROOT=/usr/local/cuda-11.7/ -DCMAKE_INSTALL_PREFIX=../install
make install

# 执行前可能需要
export LD_LIBRARY_PATH=/usr/local/opencv490/lib:/opt/tensorrt/TensorRT-8.6.1.6/lib:/usr/local/cuda-11.7/lib64:$LD_LIBRARY_PATH

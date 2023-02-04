# imufilter

编译生成动态库
``` shell
g++ imufilter.cpp rt_nonfinite.cpp -fPIC -shared  -o libimufilter.so
```

测试使用动态库
``` shell
g++ test_imufilter.cpp -L. -limufilter -o imufilter_main
```

编译生成binary并运行
``` shell
g++ test_imufilter.cpp imufilter.cpp rt_nonfinite.cpp  -std=c++11 -o test_imufilter.o
./test_imufilter.o
```
结果已验证，和matlab生成的相同

## 文件说明

`imufilter.cpp/.h` 所有的核心逻辑都在这个文件里

`rt_nonfinite.cpp/.h` 判断浮点数是否有效

`rtwtypes.h` `tmwtypes.h` 类型支持（可以优化掉）

`coder_array.h` 不依赖其他数据结构，实现多维数组，里面使用了动态内存，注意嵌入式是否支持

`test_imufilter.cpp` 用来验证核心算法的准确性，matlab输出作为真值

`pybind11_imufilter.cpp` 将C++算法封装成pybind11_imufilter python包

`test_imufilter.py` 测试pybind11_imufilter包

## pybind

``` shell
$ c++ -O3 -Wall -shared -std=c++11 -undefined dynamic_lookup $(python3 -m pybind11 --includes) pybind11_imufilter.cpp imufilter.cpp rt_nonfinite.cpp -o pybind11_imufilter$(python3-config --extension-suffix)
```

编译完毕会在当前目录下生成`pybind11_math.cpython-39-darwin.so`文件。
# FPGA Layout Project

这是一个FPGA布局布线项目，用于解决FPGA芯片上的网络路由优化问题。

## 项目概述

本项目实现了FPGA芯片上的网络路由算法，主要功能包括：
- 读取FPGA设计数据（节点、网络、拓扑结构等）
- 使用最小斯坦纳树算法或最短路算法进行路由优化
- 计算网络延迟并输出路由结果


## 目录结构

```
FPGA_pro/
├── CMakeLists.txt          # CMake构建配置文件
├── README.md               # 项目说明文档
├── a.txt                   # 思路
├── include/                # 头文件目录
│   ├── _readdata.h         # 数据读取类头文件
│   └── graphinit.h         # 图算法相关头文件
├── src/                    # 源代码目录
│   ├── main.cpp            # 主程序入口
│   ├── _readdata.cpp       # 数据读取实现
│   ├── graphinit.cpp       # 图算法实现
│   └── print_data_structures.cpp # 数据结构打印工具
├── build/                  # 构建输出目录
│   ├── bin/                # 可执行文件目录
│   │   ├── fpga_pro        # 主程序可执行文件
│   │   └── print_data_structures # 数据打印工具
│   ├── public/             # 测试数据目录
│   │   ├── checker         # 结果验证工具
│   │   ├── README.md       # 测试说明
│   │   ├── case01/         # 测试用例1
│   │   ├── case02/         # 测试用例2
│   │   ├── case03/         # 测试用例3
│   │   ├── case04/         # 测试用例4
│   │   └── sample/         # 示例测试用例
│   └── ...                 # 其他CMake生成文件
└──
```

## 核心文件说明

### 头文件
- **`include/_readdata.h`**: 定义了`ReadData`单例类，负责读取和管理FPGA设计数据
- **`include/graphinit.h`**: 定义了图算法相关的函数，包括初始化、建图、最短路算法等

### 源文件
- **`src/main.cpp`**: 程序入口点，调用`solve()`函数执行主要逻辑
- **`src/_readdata.cpp`**: 实现数据读取功能，读取FPGA输出、网络信息、拓扑结构等
- **`src/graphinit.cpp`**: 实现核心路由算法，包括最小斯坦纳树和最短路算法
- **`src/print_data_structures.cpp`**: 用于调试的数据结构打印工具

### 数据文件
每个测试用例目录包含：
- `design.fpga.out`: FPGA节点分配信息
- `design.info`: FPGA通道限制信息
- `design.net`: 网络连接信息
- `design.topo`: 拓扑结构信息
- `design.route.out`: 路由输出结果（程序生成）

## 算法说明

项目根据问题规模自动选择算法：
- **小规模问题** (FPGA数量≤8 或 网络节点数<16): 使用最小斯坦纳树算法
- **大规模问题**: 使用最短路算法



## 编译说明

### 使用CMake编译（推荐）

1. 创建构建目录并进入：
```bash
mkdir build
cd build
```

2. 配置项目：
```bash
cmake ..
```

3. 编译项目：
```bash
make
```

编译完成后，可执行文件将生成在`build/bin/`目录下。

### 运行程序

1. 运行主程序：
```bash
cd build
./bin/fpga_pro
```

2. 运行特定测试用例：
```bash
make run_case01  # 运行测试用例1
make run_case02  # 运行测试用例2
make run_case03  # 运行测试用例3
make run_case04  # 运行测试用例4
```

3. 打印数据结构（调试）：
```bash
./bin/print_data_structures
```

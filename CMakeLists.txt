cmake_minimum_required(VERSION 3.10)
project(ModbusProject)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 指定头文件路径
include_directories(include)

# 指定库文件路径
link_directories(lib)

# 添加第一个可执行文件：modbus_server
add_executable(modbus_server modbus_server.cpp)

# 链接 libmodbus 库
target_link_libraries(modbus_server modbus)

# 添加第二个可执行文件：modbus_client
add_executable(modbus_client modbus_client.cpp)

# 链接 libmodbus 库
target_link_libraries(modbus_client modbus)
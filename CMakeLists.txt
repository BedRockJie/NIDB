cmake_minimum_required(VERSION 3.10)

# 项目信息
project(NIDB VERSION 1.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 源文件
set(SOURCES main.cpp)



# 添加可执行文件
add_executable(nidb ${SOURCES})

# 链接线程库
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(Threads REQUIRED)
    target_link_libraries(nidb PRIVATE Threads::Threads)
endif()

# 配置目标属性
if(WIN32)
    # 设置应用程序图标（仅限Windows）
    set_property(TARGET nidb PROPERTY WIN32_EXECUTABLE TRUE)
    # 设置应用程序为控制台应用程序
    set_target_properties(nidb PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
endif()

# 安装规则
install(TARGETS nidb RUNTIME DESTINATION bin)

# 包含路径
include_directories(${CMAKE_SOURCE_DIR})

# 打印配置状态
message(STATUS "CMake system name: ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMake build type: ${CMAKE_BUILD_TYPE}")

cmake_minimum_required (VERSION 2.8.12)

# application name
set(application_name "STM32F4_test1")

if(NOT CMAKE_TOOLCHAIN_FILE) 
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/CMake/GNU-ARM-Toolchain.cmake)
endif(NOT CMAKE_TOOLCHAIN_FILE)

# set the build type
if(NOT CMAKE_BUILD_TYPE) 
    set(CMAKE_BUILD_TYPE Debug)
endif(NOT CMAKE_BUILD_TYPE)

if(CMAKE_BUILD_TYPE MATCHES Debug)
  message("Debug build.")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
  message("Release build.")
endif()

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# Project specific settings
project (${application_name} C CXX ASM)

# STM32-specific settings
set(DEVICE_FAMILY STM32F40_41xxx)

# 外部クロック周波数
set(HSE_VALUE 8000000)

# リンカスクリプト
get_filename_component(LINKER_SCRIPT linker/stm32f407vgt.ld ABSOLUTE)

# LTO(リンク時最適化)を使用する場合は以下を有効化
#set(ADDITIONAL_CORE_FLAGS -flto)

# add libraries
add_subdirectory(lib)

# add main project
add_subdirectory(src)

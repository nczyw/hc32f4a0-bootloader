cmake_minimum_required(VERSION 3.22)
enable_language(C ASM)

set(Drivers_Defines
)

# 驱动头文件
set(Drivers_Hearers
    "${CMAKE_CURRENT_LIST_DIR}/inc"
)

# 源文件
file(GLOB Drivers_Sources
    "${CMAKE_CURRENT_LIST_DIR}/src/*.c"
)

add_library(fatfs INTERFACE)
target_include_directories(fatfs INTERFACE ${Drivers_Hearers})
target_compile_definitions(fatfs INTERFACE ${Drivers_Defines})

# create drivers object library
add_library(fatfs_driver OBJECT)
# message(STATUS "Drivers_Sources: ${Drivers_Sources}")
target_sources(fatfs_driver PRIVATE ${Drivers_Sources})
target_link_libraries(fatfs_driver PUBLIC fatfs)

# Add libraries to the project
target_link_libraries(${CMAKE_PROJECT_NAME} fatfs_driver)
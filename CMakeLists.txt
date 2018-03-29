cmake_minimum_required(VERSION 3.0.0)
project(CVTool VERSION 0.1.0)

include(CTest)
enable_testing()

set(FLTK_DIR ./fltk)
find_package(FLTK REQUIRED NO_MODULE)
include_directories(./fltk)
include_directories(./includes)

add_executable(CVTool WIN32 ./src/main.cpp ./src/main_panel.cxx ./src/main_panel.cxx)

target_link_libraries(CVTool fltk)
target_link_libraries(CVTool ws2_32)
target_link_libraries(CVTool uavv_video)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

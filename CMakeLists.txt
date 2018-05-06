cmake_minimum_required(VERSION 3.0.0)
project(CVtool VERSION 0.1.0)

include(CTest)
enable_testing()

set(FLTK_DIR ./fltk)
find_package(FLTK REQUIRED NO_MODULE)
include_directories(./fltk)
include_directories(./includes)
include_directories(./mathgl/include)
link_directories(./mathgl/lib)

add_executable(CVtool WIN32 ./src/main.cpp ./src/ui_controller.cpp 
                            ./src/main_panel.cxx ./src/render_panel.cxx  
                            ./src/video_player.cpp ./src/xloader.c ./src/uavv_wrapper.cpp 
                            ./src/zoom_panel.cxx ./src/zoom_view.cxx 
                            ./src/klv_panel.cxx ./src/klv_view.cxx
                            ./src/draw_controller.cpp ./src/draw_model.cpp ./src/draw_thumbnail.cxx
                            ./src/plot_panel.cxx ./src/plot_drawer.cpp ./src/jsoncpp.cpp)

target_link_libraries(CVtool mgl-fltk)
target_link_libraries(CVtool mgl)
target_link_libraries(CVtool fltk)
target_link_libraries(CVtool fltk_gl)
target_link_libraries(CVtool z)
target_link_libraries(CVtool png)
target_link_libraries(CVtool jpeg)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fext-numeric-literals -fopenmp -DMGL_STATIC_DEFINE -DMGL_SRC")

if(WIN32)
    target_link_libraries(CVtool ws2_32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
    target_link_libraries(CVtool pthread)
endif()

if(APPLE)
    target_link_libraries(CVtool objc)
    target_link_libraries(CVtool "-framework CoreFoundation")
    target_link_libraries(CVtool "-framework CoreGraphics")
    target_link_libraries(CVtool "-framework CoreText")
    target_link_libraries(CVtool "-framework AppKit")
endif()


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

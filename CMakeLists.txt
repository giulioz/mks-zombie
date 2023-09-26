cmake_minimum_required (VERSION 3.7.2)

if(APPLE)
    project(mkszombie CXX C OBJC)
else()
    project(mkszombie CXX C)
endif()
set(CMAKE_CXX_STANDARD 20)

if(APPLE)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(FOUNDATION_LIBRARY Foundation)
endif()

add_subdirectory(deps/nativefiledialog-extended)
add_subdirectory(deps/portmidi)
include_directories(deps/serial/include)

set(SOURCE_FILES
	"src/main.cpp"
	"src/SynthEngine.cpp"
)

set(SERIAL_SOURCE_FILES
    "deps/serial/src/serial.cc"
    "deps/serial/include/serial/serial.h"
    "deps/serial/include/serial/v8stdint.h"
)
if(APPLE)
    # If OSX
    list(APPEND SERIAL_SOURCE_FILES deps/serial/src/impl/unix.cc)
    list(APPEND SERIAL_SOURCE_FILES deps/serial/src/impl/list_ports/list_ports_osx.cc)
elseif(UNIX)
    # If unix
    list(APPEND SERIAL_SOURCE_FILES deps/serial/src/impl/unix.cc)
    list(APPEND SERIAL_SOURCE_FILES deps/serial/src/impl/list_ports/list_ports_linux.cc)
else()
    # If windows
    list(APPEND SERIAL_SOURCE_FILES deps/serial/src/impl/win.cc)
    list(APPEND SERIAL_SOURCE_FILES deps/serial/src/impl/list_ports/list_ports_win.cc)
endif()

include(FetchContent)
FetchContent_Declare(
    hello_imgui
    GIT_REPOSITORY https://github.com/pthom/hello_imgui.git
    # Enter the desired git tag below
    # GIT_TAG
)
FetchContent_MakeAvailable(hello_imgui)
# Make cmake function `hello_imgui_add_app` available
list(APPEND CMAKE_MODULE_PATH ${HELLOIMGUI_CMAKE_PATH})
include(hello_imgui_add_app)
hello_imgui_add_app(mkszombie ${SOURCE_FILES} ${SERIAL_SOURCE_FILES})

target_link_libraries(mkszombie PRIVATE portmidi)
target_link_libraries(mkszombie PRIVATE nfd)

if(APPLE)
    target_link_libraries(mkszombie PRIVATE ${FOUNDATION_LIBRARY} ${IOKIT_LIBRARY})
elseif(UNIX)
    target_link_libraries(mkszombie PRIVATE rt pthread)
else()
    target_link_libraries(mkszombie PRIVATE setupapi)
endif()

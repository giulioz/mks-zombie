cmake_minimum_required (VERSION 3.7.2)

project(mkszombie)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(BIN_DIR ${mkszombie_SOURCE_DIR}/bin)

find_package(OpenGL REQUIRED)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

find_package(PortMidi REQUIRED)
include_directories(${PORTMIDI_INCLUDE_DIR})

# find_package(ImGui REQUIRED Sources)
# include_directories(${IMGUI_INCLUDE_DIR})
include_directories("../imgui")

set(SOURCE_FILES
	"src/main.cpp"
	"src/Piano.cpp"
)

add_executable(mkszombie 
	${SOURCE_FILES}
	"../imgui/imgui.cpp"
	"../imgui/imgui_demo.cpp"
	"../imgui/imgui_draw.cpp"
	"../imgui/imgui_tables.cpp"
	"../imgui/imgui_widgets.cpp"
	"../imgui/backends/imgui_impl_opengl3.cpp"
	"../imgui/backends/imgui_impl_sdl2.cpp"
)

target_link_libraries(mkszombie 
	${SDL2_LIBRARY}
	${PORTMIDI_LIBRARY}
	${OPENGL_LIBRARIES}
	# ImGui::ImGui
	# ImGui::Sources
)

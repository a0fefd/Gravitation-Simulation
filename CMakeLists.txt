cmake_minimum_required(VERSION 3.30)

project(N_Body_Gravitation_Simulation C)

set(PROJECT_NAME "N_Body_Gravitation_Simulation")

set(CMAKE_C_STANDARD 99)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED HINTS "C:/Program Files (x86)/GLFW")
set_target_properties(glfw PROPERTIES IMPORTED_LOCATION "C:/Program Files (x86)/GLFW/lib/libglfw3.a")
find_package(cglm REQUIRED HINTS "D:/cglm-0.9.6/")

include_directories(AFTER include/)
link_directories(lib/GLFW lib/cglm)

add_executable(${PROJECT_NAME}
        src/main.c
        src/glad.c
        src/filehandler.c
        src/shader.c

        include/main.h
        include/filehandler.h
        include/shader.h
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        OpenGL::GL
        glfw
        cglm
)
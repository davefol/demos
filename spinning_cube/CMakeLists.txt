cmake_minimum_required(VERSION 3.0.0)
project(spinning_cube VERSION 0.1.0 LANGUAGES C CXX)


set(SOURCES src/main.cpp src/glad.c include/glad/glad.h include/KHR/khrplatform.h include/fix_render_on_mac.h)
function(generate_shader_header SHADER_FILE)
    get_filename_component(SHADER_NAME ${SHADER_FILE} NAME_WE)
    set(HEADER_FILE "${CMAKE_BINARY_DIR}/generated/${SHADER_NAME}.h")
    file(READ ${SHADER_FILE} SHADER_CODE)
    string(REPLACE "\"" "\\\"" SHADER_CODE "${SHADER_CODE}")
    string(REPLACE "\n" "\\n\"\n\"" SHADER_CODE "${SHADER_CODE}")
    file(WRITE ${HEADER_FILE} "const char* ${SHADER_NAME}Shader = \"${SHADER_CODE}\\0\";\n")
endfunction()

set(SHADER_FILES
    src/spinning_cube_frag.glsl
    src/spinning_cube_vert.glsl
)

foreach(SHADER_FILE ${SHADER_FILES})
    generate_shader_header(${SHADER_FILE})
endforeach()

include_directories(${CMAKE_BINARY_DIR}/generated)

add_executable(spinning_cube ${SOURCES})
target_include_directories(
    spinning_cube 
    PRIVATE include
    ${CMAKE_BINARY_DIR}/generated
    )

find_package(glfw3 3.3 REQUIRED)
target_link_libraries(spinning_cube glfw)

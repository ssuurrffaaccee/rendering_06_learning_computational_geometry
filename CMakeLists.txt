cmake_minimum_required(VERSION 3.26)
project(
  learning_computation_geometry
  VERSION 1.0.0
  LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(glm)
find_package(glfw3)
find_package(glad)
find_package(imgui)


set(LINK_TARGET glm::glm glfw glad::glad imgui::imgui)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
endif()
include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${PROJECT_SOURCE_DIR}/external")


set(IMGUI_BINDINGS_SRC 
 "${PROJECT_SOURCE_DIR}/external/bindings/imgui_impl_glfw.cpp" 
 "${PROJECT_SOURCE_DIR}/external/bindings/imgui_impl_opengl3.cpp")

function(learn_step step_name)  
    file(GLOB_RECURSE SRC_FILES_CPP CONFIGURE_DEPENDS "src/${step_name}/*.cpp")
    file(GLOB_RECURSE CORE_SRC_FILES_CPP CONFIGURE_DEPENDS "src/Core/*.cpp")
    file(GLOB_RECURSE GRAPHIC_SRC_FILES_CPP CONFIGURE_DEPENDS "src/GraphicUtils/*.cpp")
    set(SRC_FILES ${SRC_FILES_CPP} ${CORE_SRC_FILES_CPP} ${GRAPHIC_SRC_FILES_CPP} ${IMGUI_BINDINGS_SRC})
    add_executable(${step_name} ${SRC_FILES})
    target_link_libraries(${step_name} ${LINK_TARGET})
    message("${PROJECT_SOURCE_DIR}/src/${step_name}/")
    target_include_directories(${step_name} PRIVATE  "${PROJECT_SOURCE_DIR}/src/${step_name}/")
    install(TARGETS ${step_name} RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin/${step_name})
    file(GLOB_RECURSE SHADER_FILES CONFIGURE_DEPENDS "src/GraphicUtils/*.shader" "src/${step_name}/*.ini")
    foreach(shader_path ${SHADER_FILES})
      install(
        FILES ${shader_path}
        DESTINATION ${PROJECT_SOURCE_DIR}/bin/${step_name}
      )
    endforeach()
endfunction()

learn_step(00-polygon_triangulation)
learn_step(01-convexhull_2d)
learn_step(02-convexhull_3d)
learn_step(03-voronoi_diragram)
learn_step(04-quad_tree)
learn_step(05-kd-tree)
learn_step(06-BSP)
cmake_minimum_required(VERSION 3.20)

project(poromarker)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True) 

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG bin.dbg)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE bin.rel)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO bin.relwithdbg)


find_package(imgui CONFIG REQUIRED)
find_package(OpenCV REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

include(FetchContent)

set(GITHUB_TOKEN_1 "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")
set(GITHUB_TOKEN_2 "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")

FetchContent_Declare(
  poroseg
  GIT_REPOSITORY https://${GITHUB_TOKEN_1}@github.com/s4salo/misis2023f-22-04-gavrilyuk-a-v.git
  GIT_TAG main
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/modules/Segmentation
)

FetchContent_GetProperties(poroseg)
if(NOT poroseg_POPULATED)
  FetchContent_Populate(poroseg)
  add_subdirectory(${poroseg_SOURCE_DIR}/prj.cw/ImageProcessor)
endif()

#FetchContent_Declare(
#  poroanalysis
#  GIT_REPOSITORY https://${GITHUB_TOKEN_2}@github.com/Maksim-Goncharovskiy/misis2023f-22-04-goncharovskiy-m-n.git
#  GIT_TAG main
#  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/modules/PoroAnalysis
#)

#FetchContent_GetProperties(poroanalysis)
#if(NOT poroanalysis_POPULATED)
#  FetchContent_Populate(poroanalysis)
#  add_subdirectory(${poroanalysis_SOURCE_DIR}/prj.cw/ObjData)
#endif()

add_subdirectory(modules/UIfilebr)

add_executable(poromarker "poromarker.cpp")
target_link_libraries(poromarker PRIVATE imgui::imgui opencv_core opencv_highgui opencv_imgproc imfilebrowser sabin)
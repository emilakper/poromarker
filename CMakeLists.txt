cmake_minimum_required(VERSION 3.20)

project(poromarker)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True) 

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmake")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin.dbg)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin.rel)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_CURRENT_SOURCE_DIR}/bin.relwithdbg)


find_package(imgui CONFIG REQUIRED)
find_package(OpenCV REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(FreeGLUT CONFIG REQUIRED)
find_package(implot CONFIG REQUIRED)

include(FetchContent)

set(GITHUB_TOKEN_1 "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")
set(GITHUB_TOKEN_2 "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")
set(GITHUB_TOKEN_3 "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")
set(GITHUB_TOKEN_4 "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")
option(BUILD_ONLY_LIBRARY "Building libraries" on)

FetchContent_Declare(
  poroseg
  GIT_REPOSITORY https://${GITHUB_TOKEN_1}@github.com/s4salo/misis2023f-22-04-gavrilyuk-a-v.git
  GIT_TAG main
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/modules/Segmentation
)
FetchContent_MakeAvailable(poroseg)

FetchContent_Declare(
  poroload
  GIT_REPOSITORY https://${GITHUB_TOKEN_3}@github.com/dueip/misis2023f-22-24-voevodin-e-o.git
  GIT_TAG main
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/modules/PoroLoader
)
FetchContent_MakeAvailable(poroload)

FetchContent_Declare(
  porofileui
  GIT_REPOSITORY https://${GITHUB_TOKEN_4}@github.com/emilakper/misis2023f-22-04-akperov-e-b.git
  GIT_TAG main
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/modules/FileBrowserUI
)
FetchContent_MakeAvailable(porofileui)

FetchContent_Declare(
  poroanalysis
  GIT_REPOSITORY https://${GITHUB_TOKEN_2}@github.com/Maksim-Goncharovskiy/misis2023f-22-04-goncharovskiy-m-n.git
  GIT_TAG main
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/modules/PoroAnalysis
)
FetchContent_MakeAvailable(poroanalysis)

add_executable(poromarker "poromarker.cpp")
target_link_libraries(poromarker PRIVATE imgui::imgui opencv_core opencv_highgui opencv_imgproc imfilebrowser sabin ObjData_lib loader)
target_link_libraries(poromarker PRIVATE $<IF:$<TARGET_EXISTS:FreeGLUT::freeglut>,FreeGLUT::freeglut,FreeGLUT::freeglut_static>)
target_link_libraries(poromarker PRIVATE implot::implot)
target_link_libraries(poromarker PRIVATE opencv_ml opencv_dnn opencv_core opencv_flann)

install(TARGETS poromarker
DESTINATION bin)


install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/
		DESTINATION bin
		CONFIGURATIONS Debug
		FILES_MATCHING PATTERN "*.dll*"
		)
install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/
		CONFIGURATIONS Release
		DESTINATION bin
		FILES_MATCHING PATTERN "*.dll*"
		)
install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO}/
		DESTINATION bin
		CONFIGURATIONS RelWithDebInfo
		FILES_MATCHING PATTERN "*.dll*"
		)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/testpics/ 
DESTINATION pics)
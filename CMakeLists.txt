cmake_minimum_required(VERSION 3.5)

project(STT2NG VERSION 1.0 LANGUAGES CXX)

find_package(Qt5 COMPONENTS Core Charts Widgets 3DCore 3DExtras Svg REQUIRED)

add_subdirectory(GeomRel)
add_subdirectory(STTUtil)
add_subdirectory(stt2ng)

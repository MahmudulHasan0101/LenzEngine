project(ExampleGame)

file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS "*.cpp" "*.h")

add_executable(ExampleGame ${GAME_SOURCES})
target_include_directories(ExampleGame PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ExampleGame PRIVATE Engine)

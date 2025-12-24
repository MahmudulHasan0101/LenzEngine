project(Engine)

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/*.h")

add_library(Engine STATIC ${ENGINE_SOURCES})
target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

find_package(assimp CONFIG REQUIRED)
find_package(Bullet CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)

find_package(freetype CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

find_package(nlohmann_json CONFIG REQUIRED)

find_package(plog CONFIG REQUIRED)

target_link_libraries(Engine PRIVATE
    assimp::assimp
    BulletDynamics
    BulletCollision
    Bullet3Common
    LinearMath
    glfw
    GLEW::GLEW
    freetype
    glm::glm-header-only
    nlohmann_json::nlohmann_json  
	plog::plog	
)


find_path(STB_INCLUDE_DIR "stb_image.h")
target_include_directories(Engine PRIVATE ${STB_INCLUDE_DIR})

find_path(V_HACD_INCLUDE_DIRS "VHACD.h")
target_include_directories(Engine PRIVATE ${V_HACD_INCLUDE_DIRS})





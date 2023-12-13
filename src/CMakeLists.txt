file (GLOB SOURCE_FILES "*.cpp")
file (GLOB HEADER_FILES "*.h")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
include_directories(include ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})
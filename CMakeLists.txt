cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# Set the project version and language
project(neoretro VERSION 0.1.0 LANGUAGES C CXX)
include(FetchContent)

set(NATO_PHONETIC_ALPHABET
  "Alfa" "Bravo" "Charlie" "Delta" "Echo" "Foxtrot" "Golf" "Hotel"
  "India" "Juliett" "Kilo" "Lima" "Mike" "November" "Oscar" "Papa"
  "Quebec" "Romeo" "Sierra" "Tango" "Uniform" "Victor" "Whiskey"
  "Xray" "Yankee" "Zulu"
)

# Get the patch version number from the project version
math(EXPR PATCH_INDEX "${PROJECT_VERSION_PATCH}")

# Use the patch number to select the correct word
list(GET NATO_PHONETIC_ALPHABET ${PATCH_INDEX} PROJECT_PATCH_WORD)

set(PROJECT_BUILD_NAME "Harbour ${PROJECT_PATCH_WORD}" CACHE STRING "" FORCE)
set(PROJECT_TEAM "Lywx" CACHE STRING "" FORCE)

# Get Git Information
execute_process(
    COMMAND git branch --show-current
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

set(CMAKE_PROJECT_GIT_BRANCH "${GIT_BRANCH}" CACHE STRING "Git branch" FORCE)

execute_process(
    COMMAND git rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

# Get only the first 7 characters of the hash
string(SUBSTRING "${GIT_COMMIT_HASH}" 0 7 SHORT_COMMIT_HASH)

set(CMAKE_PROJECT_GIT_COMMIT_HASH "${SHORT_COMMIT_HASH}" CACHE STRING "Git commit hash" FORCE)

execute_process(
    COMMAND git describe --tags --abbrev=0 --exact-match HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

if(NOT GIT_COMMIT_TAG)
    set(GIT_COMMIT_TAG "" CACHE STRING "Git commit tag" FORCE)
endif()

set(CMAKE_PROJECT_GIT_COMMIT_TAG "${GIT_COMMIT_TAG}" CACHE STRING "Git commit tag" FORCE)

# Set the minimum version of CMake and the deployment target for macOS
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version")

set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11 CACHE STRING "The C standard to use")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add a custom module path to locate additional CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

if (WIN32)
include(cmake/automate-vcpkg.cmake)
set(VCPKG_TRIPLET x64-windows-static)
set(VCPKG_TARGET_TRIPLET x64-windows-static)
vcpkg_bootstrap()
vcpkg_install_packages(curl)

add_compile_definitions(WIN32_LEAN_AND_MEAN NOMINMAX)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()

################################################################################
# Set target arch type if empty. Visual studio solution generator provides it.
################################################################################
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if(NOT CMAKE_VS_PLATFORM_NAME)
        set(CMAKE_VS_PLATFORM_NAME "x64")
    endif()
    message("${CMAKE_VS_PLATFORM_NAME} architecture in use")

    if(NOT ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64"
        OR "${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32"))
        message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
    endif()
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Global configuration types
################################################################################
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
endif()

################################################################################
# Sources
################################################################################
set(NEORETRO_VERSION_CODE 0 CACHE STRING "Compared against release.json's version for the update banner")
set(NEORETRO_REPOSITORY_URL "https://github.com/HarbourMasters/retro" CACHE STRING "Opened from the status bar")
set(NEORETRO_RELEASE_JSON_URL "https://raw.githubusercontent.com/HarbourMasters/retro/main/release.json" CACHE STRING "Empty disables the update check")
set(NEORETRO_RELEASES_URL "https://github.com/HarbourMasters/retro/releases/latest" CACHE STRING "Opened from the update banner")

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/src/build.cpp.in ${CMAKE_CURRENT_SOURCE_DIR}/src/build.cpp @ONLY)
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/properties.h.in ${CMAKE_CURRENT_SOURCE_DIR}/properties.h @ONLY)

# Collect source files to build the executable
file(GLOB_RECURSE ALL_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS
    "src/*.h"
    "src/*.c"
    "src/*.cpp"
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND ALL_FILES "Resource.rc")
endif()

# Create source groups that match the real file directory paths
foreach(source IN LISTS ALL_FILES)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_adjusted "${source_path}")
    source_group("${source_path_adjusted}" FILES "${source}")
endforeach()

set(CORE_FILES ${ALL_FILES})
list(FILTER CORE_FILES INCLUDE REGEX "^src/(otr|image|models|games)/|^src/services/(TextureBuilder|OtrGenerator|TextureReplacement|FontGenerator)\\.|^src/utils/(Hash|Paths|ThreadPool)\\.")
list(REMOVE_ITEM ALL_FILES ${CORE_FILES})

add_executable(${PROJECT_NAME} ${ALL_FILES})
add_library(${PROJECT_NAME}-core STATIC ${CORE_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
)
target_include_directories(${PROJECT_NAME}-core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

################################################################################
# MSVC runtime library
################################################################################
if (MSVC)
    set_target_properties(${PROJECT_NAME} ${PROJECT_NAME}-core PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
    target_compile_options(${PROJECT_NAME}-core PRIVATE /utf-8 /permissive-)
endif()

#==============================================================================#
# Dependencies                                                                 #
#==============================================================================#

#=================== spdlog ===================
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.16.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(spdlog)

#=================== raylib ===================
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(CUSTOMIZE_BUILD OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 6.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(raylib)

#=================== ImGui ===================
FetchContent_Declare(
    ImGui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.92.9-docking
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(ImGui)

FetchContent_Declare(
    rlimgui
    GIT_REPOSITORY https://github.com/raylib-extras/rlImGui.git
    GIT_TAG 1550009359ad975927f7f0e4a3f47e3f27123ea9
)
FetchContent_GetProperties(rlimgui)
if(NOT rlimgui_POPULATED)
    FetchContent_Populate(rlimgui)
endif()

add_library(ImGui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp
    ${rlimgui_SOURCE_DIR}/rlImGui.cpp
)
target_include_directories(ImGui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/misc/cpp
    ${rlimgui_SOURCE_DIR}
)
target_compile_definitions(ImGui PUBLIC IMGUI_DEFINE_MATH_OPERATORS NO_FONT_AWESOME)
target_link_libraries(ImGui PUBLIC raylib)

#=================== Clay ===================
FetchContent_Declare(
    clay
    GIT_REPOSITORY https://github.com/nicbarker/clay.git
    GIT_TAG v0.14
    GIT_SHALLOW TRUE
)
FetchContent_GetProperties(clay)
if(NOT clay_POPULATED)
    if(POLICY CMP0169)
        cmake_policy(SET CMP0169 OLD)
    endif()
    FetchContent_Populate(clay)
endif()

add_library(clay INTERFACE)
target_include_directories(clay INTERFACE ${clay_SOURCE_DIR})

#=================== Torch ===================
set(USE_STANDALONE OFF CACHE BOOL "" FORCE)
set(BUILD_STORMLIB ON CACHE BOOL "" FORCE)
foreach(TORCH_GAME SM64 MK64 SF64 PM64 FZERO BK64 MARIO_ARTIST NAUDIO OOT)
    set(BUILD_${TORCH_GAME} OFF CACHE BOOL "" FORCE)
endforeach()
FetchContent_Declare(
    torch
    GIT_REPOSITORY https://github.com/HarbourMasters/Torch.git
    GIT_TAG 72960ca16f4723f96aaf4037b76072c230af8c11
    PATCH_COMMAND ${CMAKE_COMMAND} -DPATCH=${CMAKE_SOURCE_DIR}/cmake/patches/torch-swrapper-close.patch
                  -P ${CMAKE_SOURCE_DIR}/cmake/apply-patch.cmake
)
FetchContent_MakeAvailable(torch)
target_include_directories(torch PUBLIC ${torch_SOURCE_DIR}/lib/StormLib/src)
foreach(STBIW_SYMBOL stbi_write_png stbi_write_bmp stbi_write_tga stbi_write_hdr stbi_write_jpg
        stbi_write_png_to_func stbi_write_bmp_to_func stbi_write_tga_to_func stbi_write_hdr_to_func
        stbi_write_jpg_to_func stbi_write_png_to_mem stbi_flip_vertically_on_write
        stbiw_convert_wchar_to_utf8 stbi_zlib_compress stbi_write_tga_with_rle
        stbi_write_png_compression_level stbi_write_force_png_filter)
    target_compile_definitions(N64Graphics PRIVATE ${STBIW_SYMBOL}=n64g_${STBIW_SYMBOL})
endforeach()
target_compile_definitions(torch PUBLIC USE_STORMLIB $<$<BOOL:${MSVC}>:STORMLIB_NO_AUTO_LINK>)

#=================== lodepng ===================
FetchContent_Declare(
    lodepng
    GIT_REPOSITORY https://github.com/lvandeve/lodepng.git
    GIT_TAG ed6fe5825c6a4fbb7f58ab35a4231c7543cd452a
)
FetchContent_GetProperties(lodepng)
if(NOT lodepng_POPULATED)
    FetchContent_Populate(lodepng)
endif()
add_library(lodepng STATIC ${lodepng_SOURCE_DIR}/lodepng.cpp)
target_include_directories(lodepng PUBLIC ${lodepng_SOURCE_DIR})

#=================== stb ===================
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG 2c980bb59875b0d32144a71867fbdebb2f77cd20
)
FetchContent_GetProperties(stb)
if(NOT stb_POPULATED)
    FetchContent_Populate(stb)
endif()
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})

#=================== PicoSHA2 ===================
FetchContent_Declare(
    picosha2
    GIT_REPOSITORY https://github.com/okdshin/PicoSHA2.git
    GIT_TAG 161cb3fc4170fa7a3eca9e582cebd27cc4d1fe29
)
FetchContent_GetProperties(picosha2)
if(NOT picosha2_POPULATED)
    FetchContent_Populate(picosha2)
endif()
add_library(picosha2 INTERFACE)
target_include_directories(picosha2 INTERFACE ${picosha2_SOURCE_DIR})

#=================== nlohmann_json ===================
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

#=================== nativefiledialog-extended ===================
set(NFD_INSTALL OFF CACHE BOOL "" FORCE)
set(NFD_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    nfd
    GIT_REPOSITORY https://github.com/btzy/nativefiledialog-extended.git
    GIT_TAG v1.4.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(nfd)

#=================== libcurl (optional) ===================
find_package(CURL QUIET)

target_link_libraries(${PROJECT_NAME}-core PUBLIC
    spdlog::spdlog
    torch
    storm
    N64Graphics
    BinaryTools
    zlibstatic
    lodepng
    stb
    picosha2
    nlohmann_json::nlohmann_json
)

set(ADDITIONAL_LIBRARY_DEPENDENCIES
    ${PROJECT_NAME}-core
    ImGui
    clay
    nfd
)

if (CURL_FOUND)
    list(APPEND ADDITIONAL_LIBRARY_DEPENDENCIES CURL::libcurl)
    target_compile_definitions(${PROJECT_NAME} PRIVATE NEORETRO_UPDATE_CHECK)
endif()

if (APPLE)
    list(APPEND ADDITIONAL_LIBRARY_DEPENDENCIES "-framework CoreFoundation")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ${ADDITIONAL_LIBRARY_DEPENDENCIES})

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /Od
        >
        $<$<CONFIG:Release>:
            /Oi;
            /Gy;
            /W3
        >
        /sdl-;
        /permissive-;
        /MP;
        /utf-8
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /INCREMENTAL
        >
        $<$<CONFIG:Release>:
            /OPT:REF;
            /OPT:ICF;
            /INCREMENTAL:NO
        >
        /MANIFEST:NO;
        /DEBUG;
        /SUBSYSTEM:WINDOWS
    )
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(${PROJECT_NAME}-core PRIVATE -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers)
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra -Wno-error
        -Wno-unused-parameter
        -Wno-missing-field-initializers
        -pthread
    )
    target_link_options(${PROJECT_NAME} PRIVATE -pthread)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:_DEBUG>"
        "$<$<CONFIG:Release>:NDEBUG>"
        _CRT_SECURE_NO_WARNINGS
    )
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:_DEBUG>"
        "$<$<CONFIG:Release>:NDEBUG>"
        SPDLOG_ACTIVE_LEVEL=0
    )
endif()

################################################################################
# Tests
################################################################################
option(NEORETRO_BUILD_TESTS "Build the asset pipeline tests" ON)
if (NEORETRO_BUILD_TESTS)
    enable_testing()
    add_executable(${PROJECT_NAME}-tests tests/CoreTests.cpp)
    target_link_libraries(${PROJECT_NAME}-tests PRIVATE ${PROJECT_NAME}-core)
    target_compile_definitions(${PROJECT_NAME}-tests PRIVATE NEORETRO_ASSETS_DIR="${CMAKE_SOURCE_DIR}/assets")
    add_test(NAME core COMMAND ${PROJECT_NAME}-tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

################################################################################
# Assets
################################################################################
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMENT "Copying assets..."
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
)

################################################################################
# Packaging
################################################################################
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    install(TARGETS ${PROJECT_NAME} DESTINATION . COMPONENT ${PROJECT_NAME})
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets/ DESTINATION assets COMPONENT ${PROJECT_NAME})
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    install(TARGETS ${PROJECT_NAME} DESTINATION ../MacOS COMPONENT ${PROJECT_NAME})
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets/ DESTINATION ../Resources/assets COMPONENT ${PROJECT_NAME})
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    install(TARGETS ${PROJECT_NAME} DESTINATION . COMPONENT ${PROJECT_NAME})
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets/ DESTINATION assets COMPONENT ${PROJECT_NAME})
    set_property(TARGET ${PROJECT_NAME} PROPERTY APPIMAGE_DESKTOP_FILE_TERMINAL YES)
    set_property(TARGET ${PROJECT_NAME} PROPERTY APPIMAGE_DESKTOP_FILE "${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.desktop")
    set_property(TARGET ${PROJECT_NAME} PROPERTY APPIMAGE_ICON_FILE "${CMAKE_SOURCE_DIR}/logo.png")
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(CPACK_GENERATOR "External")
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(CPACK_GENERATOR "ZIP")
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(CPACK_GENERATOR "Bundle")
endif()

set(CPACK_PROJECT_CONFIG_FILE ${CMAKE_SOURCE_DIR}/cmake/configure-packaging.cmake)
include(cmake/packaging.cmake)

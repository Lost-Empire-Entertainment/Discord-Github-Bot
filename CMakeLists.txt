cmake_minimum_required (VERSION 3.29)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PROJECT_MAJOR_VERSION 1)
set(PROJECT_MINOR_VERSION 0)
set(PROJECT_PATCH_VERSION 0)
set(PROJECT_VERSION "${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_VERSION}")

project("Discord Github Bot" VERSION ${PROJECT_VERSION})

# Paths to frequently used directories
set(RESOURCE_FILE "${CMAKE_SOURCE_DIR}/files/icon.rc")

# Header files
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

# External files
set(EXT_GLAD_DIR "${CMAKE_SOURCE_DIR}/_external_shared/Glad")
set(EXT_GLFW_DIR "${CMAKE_SOURCE_DIR}/_external_shared/GLFW")
set(EXT_GLM_DIR "${CMAKE_SOURCE_DIR}/_external_shared/GLM")
set(EXT_IMGUI_DIR "${CMAKE_SOURCE_DIR}/_external_shared/ImGui")
set(EXT_MAGIC_ENUM_DIR "${CMAKE_SOURCE_DIR}/_external_shared/Magic_enum")
set(EXT_STB_IMAGE_DIR "${CMAKE_SOURCE_DIR}/_external_shared/Stb_image")
set(EXT_CURL_DIR "${CMAKE_SOURCE_DIR}/_external_shared/libcurl")
set(EXT_DPP_DIR "${CMAKE_SOURCE_DIR}/_external_shared/dpp")
set(EXT_DOTENV_DIR "${CMAKE_SOURCE_DIR}/_external_shared/dotenv-cpp")
set(EXT_ZLIB_DIR "${CMAKE_SOURCE_DIR}/_external_shared/ZLib")
set(EXT_OPENSSL_DIR "${CMAKE_SOURCE_DIR}/_external_shared/OpenSSL")
set(EXT_OPUS_DIR "${CMAKE_SOURCE_DIR}/_external_shared/Opus")

# Add GLAD
add_library(glad STATIC ${EXT_GLAD_DIR}/glad.c)
target_include_directories(glad PUBLIC ${EXT_GLAD_DIR}/..)

# Add GLFW
set(GLFW_LIBRARY_PATH "${EXT_GLFW_DIR}/glfw3.lib")

# Add magic_enum
add_library(magic_enum INTERFACE)
target_include_directories(magic_enum INTERFACE ${EXT_MAGIC_ENUM_DIR})

# Group external libraries
add_library(external_libs INTERFACE)
target_link_libraries(external_libs INTERFACE glad magic_enum)

# Add your source files
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
	${CMAKE_SOURCE_DIR}/src/*.cpp
	${CMAKE_SOURCE_DIR}/src/*/*.cpp
)

# Add external source files
list(APPEND SOURCE_FILES
	${EXT_GLAD_DIR}/glad.c
	${EXT_IMGUI_DIR}/imgui.cpp
	${EXT_IMGUI_DIR}/imgui_draw.cpp
	${EXT_IMGUI_DIR}/imgui_impl_glfw.cpp
	${EXT_IMGUI_DIR}/imgui_impl_opengl3.cpp
	${EXT_IMGUI_DIR}/imgui_tables.cpp
	${EXT_IMGUI_DIR}/imgui_widgets.cpp
	${EXT_STB_IMAGE_DIR}/stb_image.cpp
)

# Add the executable
add_executable(Discord_Github_Bot ${SOURCE_FILES} ${RESOURCE_FILE})

# Forces the name to be Discord Github Bot
set_target_properties(Discord_Github_Bot PROPERTIES OUTPUT_NAME "Discord Github Bot")

target_compile_features(Discord_Github_Bot PRIVATE cxx_std_20)

target_include_directories(Discord_Github_Bot PRIVATE 
	${INCLUDE_DIR}
	${EXT_GLAD_DIR}
	${EXT_GLFW_DIR}
	${EXT_GLM_DIR}
	${EXT_MAGIC_ENUM_DIR}
	${EXT_IMGUI_DIR}
	${EXT_STB_IMAGE_DIR}
	${EXT_CURL_DIR}/include
	${EXT_DPP_DIR}
	${EXT_DOTENV_DIR}
)

target_link_libraries(Discord_Github_Bot PRIVATE external_libs ${GLFW_LIBRARY_PATH} ${EXT_CURL_DIR}/libcurl_imp.lib ${EXT_DPP_DIR}/dpp.lib)

# Define GLFW_INCLUDE_NONE
target_compile_definitions(Discord_Github_Bot PRIVATE GLFW_INCLUDE_NONE)

# Set the subsystem to WINDOWS
#set_target_properties(Discord_Github_Bot PROPERTIES
#    WIN32_EXECUTABLE TRUE
#)

# Set the entry point (MSVC-specific)
#if (MSVC)
#	set_target_properties(Discord_Github_Bot PROPERTIES
#    LINK_FLAGS "/ENTRY:mainCRTStartup"
#)
#endif()

# Set the folder inside the install folder where the exe will be placed for this project
set(CMAKE_INSTALL_BINDIR bin)
install(TARGETS Discord_Github_Bot DESTINATION ${CMAKE_INSTALL_BINDIR})

#
# DLL FILES START
#

# Copy libcurl.dll to the output directory after build
add_custom_command(TARGET Discord_Github_Bot POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove
    $<TARGET_FILE_DIR:Discord_Github_Bot>/libcurl.dll
	COMMAND ${CMAKE_COMMAND} -E copy
    ${EXT_CURL_DIR}/libcurl.dll
    $<TARGET_FILE_DIR:Discord_Github_Bot>/libcurl.dll
)

# Copy dpp.dll to the output directory after build
add_custom_command(TARGET Discord_Github_Bot POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove
    $<TARGET_FILE_DIR:Discord_Github_Bot>/dpp.dll
	COMMAND ${CMAKE_COMMAND} -E copy
    ${EXT_DPP_DIR}/dpp.dll
    $<TARGET_FILE_DIR:Discord_Github_Bot>/dpp.dll
)

# Copy zlib1.dll to the output directory after build
add_custom_command(TARGET Discord_Github_Bot POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove
    $<TARGET_FILE_DIR:Discord_Github_Bot>/zlib1.dll
	COMMAND ${CMAKE_COMMAND} -E copy
    ${EXT_ZLIB_DIR}/zlib1.dll
    $<TARGET_FILE_DIR:Discord_Github_Bot>/zlib1.dll
)

# Copy libssl-1_1-x64.dll to the output directory after build
add_custom_command(TARGET Discord_Github_Bot POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove
    $<TARGET_FILE_DIR:Discord_Github_Bot>/libssl-1_1-x64.dll
	COMMAND ${CMAKE_COMMAND} -E copy
    ${EXT_OPENSSL_DIR}/libssl-1_1-x64.dll
    $<TARGET_FILE_DIR:Discord_Github_Bot>/libssl-1_1-x64.dll
)

# Copy libcrypto-1_1-x64.dll to the output directory after build
add_custom_command(TARGET Discord_Github_Bot POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove
    $<TARGET_FILE_DIR:Discord_Github_Bot>/libcrypto-1_1-x64.dll
	COMMAND ${CMAKE_COMMAND} -E copy
    ${EXT_OPENSSL_DIR}/libcrypto-1_1-x64.dll
    $<TARGET_FILE_DIR:Discord_Github_Bot>/libcrypto-1_1-x64.dll
)

# Copy opus.dll to the output directory after build
add_custom_command(TARGET Discord_Github_Bot POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove
    $<TARGET_FILE_DIR:Discord_Github_Bot>/opus.dll
	COMMAND ${CMAKE_COMMAND} -E copy
    ${EXT_OPUS_DIR}/opus.dll
    $<TARGET_FILE_DIR:Discord_Github_Bot>/opus.dll
)

#
# DLL FILES END
#

# Copy "data.env" to install directory after build
add_custom_command(TARGET Discord_Github_Bot POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove
    $<TARGET_FILE_DIR:Discord_Github_Bot>/data.env
	COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/data.env
    $<TARGET_FILE_DIR:Discord_Github_Bot>/data.env
)

# Copy "files" directory to the install directory after build
add_custom_command(TARGET Discord_Github_Bot POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove_directory
    $<TARGET_FILE_DIR:Discord_Github_Bot>/files
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/files $<TARGET_FILE_DIR:Discord_Github_Bot>/files
)

# Copy "LICENSE.md" to install directory after build
add_custom_command(TARGET Discord_Github_Bot POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove
    $<TARGET_FILE_DIR:Discord_Github_Bot>/LICENSE.md
	COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/LICENSE.md
    $<TARGET_FILE_DIR:Discord_Github_Bot>/LICENSE.md
)

# Copy "README.md" to install directory after build
add_custom_command(TARGET Discord_Github_Bot POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove
    $<TARGET_FILE_DIR:Discord_Github_Bot>/README.md
	COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/README.md
    $<TARGET_FILE_DIR:Discord_Github_Bot>/README.md
)

# Copy "LIBRARIES.md" to install directory after build
add_custom_command(TARGET Discord_Github_Bot POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E remove
    $<TARGET_FILE_DIR:Discord_Github_Bot>/LIBRARIES.md
	COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/LIBRARIES.md
    $<TARGET_FILE_DIR:Discord_Github_Bot>/LIBRARIES.md
)

# Include CPack to enable packaging
include(CPack)
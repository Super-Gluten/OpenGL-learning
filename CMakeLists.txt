# 1. CMake 版本要求
# --------------------------------
cmake_minimum_required(VERSION 3.15)
cmake_policy(VERSION 3.15)

# 2. 项目定义
# --------------------------------
project(LearnOpenGL)

# 3. C++ 标准设置
# --------------------------------
# 设置 C++ 标准为 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 4. 构建类型配置
# --------------------------------
# 设置编译器模式为 Debug
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# ===================== 编译器自动检测 =====================
# 区分 MSVC（VS）和 GNU（MinGW）
if (MSVC)
    set(IS_MSVC ON)
    set(IS_MINGW OFF)
    message(STATUS "Detected compiler: MSVC (Visual Studio)")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(IS_MSVC OFF)
    set(IS_MINGW ON)
    message(STATUS "Detected compiler: GNU (MinGW)")
else()
    message(WARNING "Unsupported compiler: ${CMAKE_CXX_COMPILER_ID}, fallback to MinGW config")
    set(IS_MSVC OFF)
    set(IS_MINGW ON)
endif()

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 查找OpenGL
find_package(OpenGL REQUIRED)

# ===================== FetchContent 拉取所有依赖 =====================
include(FetchContent)

# 1. GLFW 
FetchContent_Declare(
    glfw
    URL https://github.com/glfw/glfw/releases/download/3.3.8/glfw-3.3.8.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# 2. ImGui 
FetchContent_Declare(
    imgui
    URL https://github.com/ocornut/imgui/archive/refs/tags/v1.90.4.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(imgui)

# 3. GLM 
FetchContent_Declare(
    glm
    URL https://github.com/g-truc/glm/archive/refs/tags/1.0.0.zip 
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(glm)

# 4. Assimp
FetchContent_Declare(
    assimp
    URL https://github.com/assimp/assimp/archive/refs/tags/v6.0.4.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# 禁用Assimp不必要的组件，加快编译
set(ASSIMP_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB ON CACHE BOOL "" FORCE)  # 内置zlib，避免依赖问题
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)   # 只保留导入功能
FetchContent_MakeAvailable(assimp)

# ===================== 源文件配置 =====================
# ImGui 源文件 (原有，保留)
set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

# 定义STB_IMAGE_IMPLEMENTATION来生成实现
set(STB_SOURCE
    ${CMAKE_SOURCE_DIR}/src/stb_image/stb_image_wrapper.cpp  # 新增一个包装文件，见下方说明
)

set(GLAD_SOURCE_DIR
    ${CMAKE_SOURCE_DIR}/build/_deps/glad
)

# 创建可执行文件
add_executable(${PROJECT_NAME} 
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${IMGUI_SOURCES}
    ${STB_SOURCE} 
    ${GLAD_SOURCE_DIR}/src/glad.c
)

# ===================== 包含目录配置 =====================
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include          # 项目自有头文件
    ${imgui_SOURCE_DIR}                  # ImGui核心头文件
    ${imgui_SOURCE_DIR}/backends         # ImGui后端头文件
    ${glm_SOURCE_DIR}                    # GLM头文件
    ${CMAKE_SOURCE_DIR}/src/stb_image    # stb头文件
    ${assimp_SOURCE_DIR}/include         # Assimp头文件
    ${GLAD_SOURCE_DIR}/include           # Glad头文件
)

# ===================== 链接库配置 =====================
target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL          # OpenGL核心库
    glfw                # GLFW库
    glm::glm            # GLM库
    assimp::assimp      # Assimp库
)

# ===================== 编译定义 =====================
target_compile_definitions(${PROJECT_NAME} PRIVATE
    IMGUI_IMPL_OPENGL_LOADER_GLAD    # ImGui使用Glad加载OpenGL
    _HAS_STD_BYTE=0                  # 解决byte歧义
    NOMINMAX                         # 避免Windows min/max宏冲突
)

# ===================== 后置构建命令 =====================
# 复制GLFW DLL
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:glfw>
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

# 复制 Assimp DLL
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:assimp>
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

# 复制 Shader文件 
file(GLOB SHADER
    ${CMAKE_SOURCE_DIR}/include/Shader/*.vs
    ${CMAKE_SOURCE_DIR}/include/Shader/*.fs
)
foreach(SHADER ${SHADERS})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SHADER} 
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endforeach(SHADER)
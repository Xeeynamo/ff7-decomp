cmake_minimum_required(VERSION 3.10..3.31)

project(ff7)

set(CMAKE_C_STANDARD 11)

# PC port: only the boot path is wired up so far. The remaining engine
# translation units are commented out until their dependencies are ported;
# src/pc/stubs.c stands in for the symbols the boot path still references.
set(FF7_SOURCES
    src/pc/main.c
    src/pc/unzip.c
    src/pc/stubs.c
    src/pc/cdrom.c
    src/pc/title_data.c
    src/pc/menu_data.c

    # Engine
    src/main/110B8.c
    src/main/33B70.c
    src/main/1F6B4.c
    src/main/ovl.c
    src/main/1CDA4.c
    src/main/26B70.c
    src/main/input.c
    src/main/menutable.c
    src/main/144D8.c
    src/main/14C70.c
    src/main/15B44.c
    src/main/17238.c
    src/main/1C0EC.c
    src/main/48F60.c
    src/main/akao.c
    src/main/btlinit.c

    # Menu
    src/menu/title.c
    src/menu/savemenu.c
    src/menu/cnfgmenu.c
    src/menu/bginmenu.c

    # Misc
    src/ending/ending.c
)

add_executable(str tools/str.c)

set(FF7_STR_SOURCES)
foreach(src ${FF7_SOURCES})
    set(strd_src "${CMAKE_CURRENT_BINARY_DIR}/strd/${src}")
    get_filename_component(strd_dir ${strd_src} DIRECTORY)
    get_filename_component(src_dir ${src} DIRECTORY)
    add_custom_command(
        OUTPUT ${strd_src}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${strd_dir}"
        COMMAND $<TARGET_FILE:str> < ${CMAKE_CURRENT_SOURCE_DIR}/${src} > ${strd_src}
        DEPENDS ${src} str
    )
    set_source_files_properties(${strd_src} PROPERTIES
        INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/${src_dir}")
    list(APPEND FF7_STR_SOURCES ${strd_src})
endforeach()

add_subdirectory(tools/psyz/psyz)
target_include_directories(psyz PUBLIC tools/psyz/psyz/include)
target_compile_definitions(psyz PRIVATE __psyz)

add_executable(${PROJECT_NAME} ${FF7_STR_SOURCES})
psyz_strip_file_prefix(${PROJECT_NAME})
psyz_title(${PROJECT_NAME} "Final Fantasy VII")
find_package(ZLIB REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE psyz m ZLIB::ZLIB)
# The PC build resolves the PSX SDK headers from psyz (via the psyz target's
# PUBLIC include dir), so include/psxsdk is intentionally left off the path.
target_include_directories(${PROJECT_NAME} PRIVATE include src/pc)
target_compile_definitions(${PROJECT_NAME} PRIVATE SKIP_ASM=1 NON_MATCHINGS FF7_STR VERSION_PC)
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wno-implicit-function-declaration
    -Wno-int-conversion
    -Wno-return-mismatch
    -Wno-incompatible-pointer-types
    -Wno-builtin-declaration-mismatch
    -Werror=return-local-addr
    -Werror=sizeof-array-argument
    -Werror=switch-bool
    -Werror=unused-result
    -Werror=pointer-compare
    -Werror=div-by-zero
    -Werror=overflow
    -Werror=shift-count-negative
    -Werror=shift-count-overflow
    -Werror=discarded-qualifiers
    -Werror=stringop-overflow
    -Werror=stringop-overread
    -Werror=switch-outside-range
    -Werror=switch-unreachable
)

# Opt-in AddressSanitizer for the PC build: cmake -B build/pc -DFF7_ASAN=ON
option(FF7_ASAN "Build the PC executable with AddressSanitizer" OFF)
if(FF7_ASAN)
    target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address -fno-omit-frame-pointer -g)
    target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address)
endif()

cmake_minimum_required(VERSION 3.10..3.31)

project(ff7 C ASM)

set(CMAKE_C_STANDARD 11)

set(FF7_PSYZ
    src/pc/main.c
    src/pc/unzip.c
    src/pc/stubs.c
    src/pc/lzs.c
)

set(FF7_MAIN
    src/main/110B8.c
    src/main/144D8.c
    src/main/14C70.c
    src/main/15B44.c
    src/main/17238.c
    src/main/1C0EC.c
    src/main/1CDA4.c
    src/main/1F6B4.c
    src/main/26B70.c
    src/main/48F60.c
    src/main/akao_init.c
    src/main/akao.c
    src/main/btlinit.c
    src/main/btlrand.c
    src/main/cd.c
    src/main/input.c
    src/main/menutable.c
    src/main/ovl.c
)

set(FF7_MENU
    src/menu/bginmenu.c
    src/menu/cnfgmenu.c
    #src/menu/itemmenu.c
    src/menu/savemenu.c
    src/menu/title.c
)

set(FF7_MISC
    src/brom/brom.c
    src/dschange/dschange.c
    src/ending/ending.c
)

set(FF7_BATTLE
    src/battle/batini.c
    src/battle/batres.c
    src/battle/battle1.c
    src/battle/battle2.c
    src/battle/battle3.c
    src/battle/battle.c
)

set(FF7_FIELD
    src/field/field_dialog.c
    src/field/field_debug.c
    src/field/field_fileInfo.c
    src/field/field_main.c
    src/field/field_loop.c
    src/field/field_bg.c
    src/field/field_rand.c
    src/field/field_event.c
    src/field/field_opcodes.c
)

set(FF7_WORLD
    src/world/world2.c
    src/world/world.c
)

set(FF7_MINI
    src/mini/chocobo/chocobo.c
    src/mini/chocobo/chocobo2.c
    src/mini/chocobo/chocobo_model.c
)

set(FF7_MAGIC
    src/magic/barrier.c
    src/magic/brizad.c
    src/magic/lv5deth.c
    src/magic/mabaria.c
    src/magic/thunder.c
)

add_executable(str tools/str.c)

# Runs every source through tools/str and stores the resulting paths in out_var
function(ff7_str out_var)
    set(out)
    foreach(src ${ARGN})
        if(src MATCHES "\\.c$")
            list(APPEND out "${CMAKE_CURRENT_BINARY_DIR}/strd/${src}")
        else()
            list(APPEND out "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
        endif()
    endforeach()
    set(${out_var} ${out} PARENT_SCOPE)
endfunction()

set(FF7_ALL ${FF7_PSYZ} ${FF7_MAIN} ${FF7_MENU} ${FF7_MISC} ${FF7_BATTLE}
    ${FF7_FIELD} ${FF7_WORLD} ${FF7_MINI} ${FF7_MAGIC})
list(REMOVE_DUPLICATES FF7_ALL)
list(FILTER FF7_ALL INCLUDE REGEX "\\.c$")
foreach(src ${FF7_ALL})
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
    list(APPEND FF7_STR_ALL ${strd_src})
endforeach()
# Both targets share the generated sources, so they are produced once here
add_custom_target(${PROJECT_NAME}_strd DEPENDS ${FF7_STR_ALL})

foreach(group PSYZ MAIN MENU MISC BATTLE FIELD WORLD MINI MAGIC)
    ff7_str(FF7_STR_${group} ${FF7_${group}})
endforeach()

add_subdirectory(tools/psyz/psyz)
target_include_directories(psyz PUBLIC tools/psyz/psyz/include)
target_compile_definitions(psyz PRIVATE __psyz)
psyz_title(psyz "Final Fantasy VII")
find_package(ZLIB REQUIRED)

function(ff7_configure target)
    add_dependencies(${target} ${PROJECT_NAME}_strd)
    psyz_strip_file_prefix(${target})
    target_link_libraries(${target} PRIVATE psyz m ZLIB::ZLIB)
    # The PC build resolves the PSX SDK headers from psyz (via the psyz target's
    # PUBLIC include dir), so include/psxsdk is intentionally left off the path.
    target_include_directories(${target} PRIVATE include src/pc)
    target_compile_definitions(${target} PRIVATE SKIP_ASM=1 NON_MATCHINGS FF7_STR PLATFORM_PSYZ)
    target_compile_options(${target} PRIVATE
        -Wa,-I${CMAKE_CURRENT_SOURCE_DIR}
        -Wa,-I${CMAKE_CURRENT_SOURCE_DIR}/include
        -fcommon
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
    if(FF7_ASAN)
        target_compile_options(${target} PRIVATE -fsanitize=address -fno-omit-frame-pointer -g)
        target_link_options(${target} PRIVATE -fsanitize=address)
    endif()
endfunction()

option(FF7_ASAN "Build the PC executable with AddressSanitizer" OFF)

add_executable(${PROJECT_NAME}
    ${FF7_STR_PSYZ} ${FF7_STR_MAIN} ${FF7_STR_MENU} ${FF7_STR_MISC})
ff7_configure(${PROJECT_NAME})

add_library(${PROJECT_NAME}_lib
    ${FF7_STR_MAIN} ${FF7_STR_MENU} ${FF7_STR_MISC} ${FF7_STR_BATTLE}
    ${FF7_STR_FIELD} ${FF7_STR_WORLD} ${FF7_STR_MINI} ${FF7_STR_MAGIC})
set_target_properties(${PROJECT_NAME}_lib PROPERTIES OUTPUT_NAME ${PROJECT_NAME})
ff7_configure(${PROJECT_NAME}_lib)

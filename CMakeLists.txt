cmake_minimum_required(VERSION 3.10..3.31)

project(ff7)

set(CMAKE_C_STANDARD 11)

set(FF7_SOURCES
    # Engine
    src/main/110B8.c
    src/main/144D8.c
    src/main/14C70.c
    src/main/15B44.c
    src/main/17238.c
    src/main/1C0EC.c
    src/main/1CDA4.c
    src/main/1F6B4.c
    src/main/269C0.c
    src/main/26B70.c
    src/main/akao.c
    src/main/btlinit.c
    src/main/btlrand.c
    src/main/input.c
    src/main/menutable.c
    src/main/ovl.c
    src/main/psxsdk.c
    src/main/unzip.c

    # Menu
    src/menu/bginmenu.c
    src/menu/cnfgmenu.c
    src/menu/itemmenu.c
    src/menu/savemenu.c
    src/menu/title.c

    # Misc
    src/brom/brom.c
    src/dschange/dschange.c
    src/ending/ending.c

    # Battle
    src/battle/batini.c
    src/battle/batres.c
    src/battle/battle1.c
    src/battle/battle2.c
    src/battle/battle3.c
    src/battle/battle.c

    # Field
    src/field/field2.c
    src/field/field.c
    src/field/field_event.c
    src/field/field_opcodes.c

    # World
    src/world/world2.c
    src/world/world.c

    # Minigames
    src/mini/chocobo/chocobo.c

    # Magics
    src/magic/barrier.c
    src/magic/brizad.c
    src/magic/lv5deth.c
    src/magic/mabaria.c
    src/magic/thunder.c
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

add_library(${PROJECT_NAME} ${FF7_STR_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE include include/psxsdk)
target_compile_definitions(${PROJECT_NAME} PRIVATE SKIP_ASM=1 FF7_STR)
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

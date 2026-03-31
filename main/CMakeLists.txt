idf_component_register(SRCS "PagerOS.cpp" "hal.cpp" "os_elf.cpp" "psram_alloc.cpp"
                    INCLUDE_DIRS ".")
target_compile_options(${COMPONENT_TARGET} PRIVATE "-Wno-error" "-Wno-format")

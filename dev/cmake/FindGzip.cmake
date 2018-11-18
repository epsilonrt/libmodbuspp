# Find gzip
# Export variables:
#  GZIP_FOUND
#  GZIP_EXE

find_program(GZIP_EXE gzip)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS( gzip DEFAULT_MSG GZIP_EXE )
mark_as_advanced(GZIP_EXE)

macro(gzip_compress target_name input_files result_archive)
    add_custom_command(
        OUTPUT ${result_archive}
        COMMAND ${GZIP_EXE} --best --to-stdout ${input_files} > ${result_archive}
        COMMENT "Compressing ${input_files}"
        DEPENDS ${input_files} ${ARGN}
    )
    add_custom_target(${target_name} ALL DEPENDS ${result_archive})
endmacro()

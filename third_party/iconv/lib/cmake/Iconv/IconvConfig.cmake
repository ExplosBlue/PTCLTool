add_library(Iconv::Iconv SHARED IMPORTED)
set_target_properties(Iconv::Iconv PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/../../iconv.dll"
    IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/../../iconv.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/../../../include"
)
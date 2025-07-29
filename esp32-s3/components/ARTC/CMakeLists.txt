idf_component_register(
    INCLUDE_DIRS "include"
)
 
add_prebuilt_library(alirtc_sdk "${CMAKE_CURRENT_SOURCE_DIR}/lib/${CONFIG_IDF_TARGET}/libalirtc_sdk.a" PRIV_REQUIRES lwip)
 
target_link_libraries(${COMPONENT_LIB} INTERFACE alirtc_sdk -v)

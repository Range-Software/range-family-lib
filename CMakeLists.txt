# Create a static library
qt_add_library(range-family-lib
    STATIC
        src/rfl_address.cpp
        src/rfl_date.cpp
        src/rfl_event.cpp
        src/rfl_geo_coordinate.cpp
        src/rfl_name.cpp
        src/rfl_person.cpp
        src/rfl_picture.cpp
        src/rfl_place.cpp
        src/rfl_relation.cpp
        src/rfl_tool_action.cpp
        src/rfl_tree.cpp
        src/rfl_tree_diff.cpp
        src/rfl_tree_model.cpp
        src/rfl_tree_model_item.cpp
        include/rfl_address.h
        include/rfl_date.h
        include/rfl_event.h
        include/rfl_geo_coordinate.h
        include/rfl_name.h
        include/rfl_person.h
        include/rfl_picture.h
        include/rfl_place.h
        include/rfl_relation.h
        include/rfl_tool_action.h
        include/rfl_tree.h
        include/rfl_tree_diff.h
        include/rfl_tree_model.h
        include/rfl_tree_model_item.h
        include/rfl_utils.h
)

# Include directories
target_include_directories(range-family-lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(range-family-lib
    PUBLIC
        range-base-lib
        common_defines

)

qt_add_translations(range-family-lib
    TS_FILES
        translations/en.ts
        translations/sk.ts
    RESOURCE_PREFIX /i18n/range-family-lib
)

### Set each program separatly.
# mask raster
SET(MASKFILES mask_rasters.cpp)
# combine raster
SET(COMBINEFILES CombineRaster.cpp CombineRasterMain.cpp)
# reclassify raster
SET(RECLASSIFYFILES reclassify_raster.cpp)
# import raster to MongoDB if found mongo-c-driver
IF (BSON_FOUND AND MONGOC_FOUND)
    SET(IMPORTMONGOFILES import_raster.cpp)
ENDIF ()

IF (BSON_FOUND AND MONGOC_FOUND)
    geo_include_directories(${BSON_INCLUDE_DIR} ${MONGOC_INCLUDE_DIR})
ENDIF ()
IF (GDAL_FOUND)
    INCLUDE(SetFlagGDAL)
    geo_include_directories(${GDAL_INCLUDE_DIR})
ENDIF ()
geo_include_directories(${CCGL_DIR})

ADD_EXECUTABLE(mask_raster ${MASKFILES})
ADD_EXECUTABLE(combine_raster ${COMBINEFILES})
ADD_EXECUTABLE(reclassify_raster ${RECLASSIFYFILES})
IF (BSON_FOUND AND MONGOC_FOUND)
    ADD_EXECUTABLE(import_raster ${IMPORTMONGOFILES})
ENDIF ()

SET(APPS_TARGETS mask_raster
                 combine_raster
                 reclassify_raster
                )
IF (BSON_FOUND AND MONGOC_FOUND)
    list(APPEND APPS_TARGETS import_raster)
ENDIF ()

foreach (c_target ${APPS_TARGETS})
    SET_TARGET_PROPERTIES(${c_target} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    target_link_libraries(${c_target} ${TARGET_VISIBILITY} ${CCGLNAME})
    IF (BSON_FOUND AND MONGOC_FOUND)
        TARGET_LINK_LIBRARIES(${c_target} ${TARGET_VISIBILITY} ${BSON_LIBRARIES} ${MONGOC_LIBRARIES})
    ENDIF ()
    IF (GDAL_FOUND)
        TARGET_LINK_LIBRARIES(${c_target} ${TARGET_VISIBILITY} ${GDAL_LIBRARIES})
        SET_TARGET_PROPERTIES(${c_target} PROPERTIES LINKER_LANGUAGE CXX)
    ENDIF ()
    ### For Xcode generator, specific LLVM-Clang installed by brew, and add link library of OpenMP explicitly.
    IF(CV_CLANG AND LLVM_VERSION_MAJOR)
        TARGET_LINK_LIBRARIES(${c_target} ${TARGET_VISIBILITY} ${OpenMP_LIBRARY})
    ENDIF()
    install(TARGETS ${c_target} DESTINATION ${INSTALL_DIR})
    LIST(APPEND APP_TARGETS ${c_target})
    IF (MSVC OR XCODE)
        SET_PROPERTY(TARGET ${c_target} PROPERTY FOLDER "apps")
    ENDIF ()
endforeach (c_target ${APPS_TARGETS})

SET(APP_TARGETS "${APP_TARGETS}" PARENT_SCOPE)

project(mcfio)

add_compile_options(-Wno-error=incompatible-function-pointer-types)
add_library(${PROJECT_NAME}  src/mcfio_FPrintDictionary.F src/mcf_evt_xdr.c src/mcfio_FBinding.c src/mcfio_Util1.c
        src/mcfio_Direct.c src/mcfio_SeqDummy.c src/mcfio_Block.c src/mcf_ntubldInit.c src/mcf_ntuBldDbinc.c
        src/mcf_NTuIOFiles.c  src/mcf_NTuIOUtils.c src/mcfio_UserDictionary.c )
target_link_libraries(${PROJECT_NAME} PUBLIC)

message(STATUS "mcfio: ${PROJECT_NAME}  dir: ${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src )
install(TARGETS ${PROJECT_NAME})


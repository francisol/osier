
include_directories(../lua)
AUX_SOURCE_DIRECTORY( . SRC)
add_library(lfs ${SRC})
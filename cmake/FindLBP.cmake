file(GLOB LBP_SRCS
    /usr/local/include/package_lbp/*.h
    )

find_path(LBP_INCLUDE_DIRS ${LBP_SRCS}
	"/usr/include"
	"/usr/local/include/package_lbp/"
	)
find_library(LBP_LIBRARIES liblbp.so
	"/usr/lib"
	"/usr/local/lib"
	)
include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(lbp
	FOUND_VAR lbp_FOUND
	REQUIRED_VARS LBP_INCLUDE_DIRS LBP_LIBRARIES
	)

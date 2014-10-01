# ESE Intercomm
#
# @author Natesh Narain
#

cmake_minimum_required(VERSION 2.6)

set(name "Intercom")
project(${name})

# find dependencies
find_package(Qt5C REQUIRED)

# include directories
set(INCL "include/")
set(SRC "src/")

# find headers
file(GLOB_RECURSE HEADER "${INCL}*.h")
file(GLOB SOURCE "${src}*.c")

# application
add_executable(
	${name}
	${SOURCE}
	${HEADER}
)

# link libraries
#
# SPDX-License-Identifier: CC0-1.0

# Tests are listed in order of complexity.
# They are not alphabetically sorted.
camera_tests = [
#    [ 'capture',                'capture1.cpp' ],	#Create cameramanager, start camera
#    [ 'capture',                'capture2.cpp' ],	#+ stop camera
#    [ 'capture',                'capture3.cpp' ],	#+ get camera ID, Acquire camera
#    [ 'capture',                'capture4.cpp' ],	#+ configure camera default settings and validate, stop camera
#    [ 'capture',                'capture5.cpp' ],	#+ Allocate buffers
#    [ 'capture',                'capture6.cpp' ],	#+ Frame capure request creation, Enable signal & slot, start capturing data, start eventdispatcher for event notications
#    [ 'capture',                'capture7.cpp' ],	#+ prints buffer stats like FPS, seq num, bytesused and timestamp
    [ 'capture',                'capture8.cpp' ],	#+ Add frame dump logice with macro option #define DUMP_BUFFER

]

foreach t : camera_tests
    exe = executable(t[0], t[1],
                     dependencies : libcamera_dep,
                     link_with : test_libraries,
                     include_directories : test_includes_internal)
    test(t[0], exe, suite : 'camera', is_parallel : false)
endforeach

# SPDX-License-Identifier: CC0-1.0

# Tests are listed in order of complexity.
# They are not alphabetically sorted.
camera_tests = [
#    [ 'capture',                'capture1.cpp' ],	#Create cameramanager, start camera
#    [ 'capture',                'capture2.cpp' ],	#+ stop camera
#    [ 'capture',                'capture3.cpp' ],	#+ get camera ID, Acquire camera
#    [ 'capture',                'capture4.cpp' ],	#+ configure camera default settings and validate, stop camera
    [ 'capture',                'capturei5.cpp' ],	#+ Allocate buffers

]

foreach t : camera_tests
    exe = executable(t[0], t[1],
                     dependencies : libcamera_dep,
                     link_with : test_libraries,
                     include_directories : test_includes_internal)
    test(t[0], exe, suite : 'camera', is_parallel : false)
endforeach

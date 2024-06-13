# CMake generated Testfile for 
# Source directory: /mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/opencv_contrib-4.x/modules/img_hash
# Build directory: /mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/build/modules/img_hash
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(opencv_test_img_hash "/mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/build/bin/opencv_test_img_hash" "--gtest_output=xml:opencv_test_img_hash.xml")
set_tests_properties(opencv_test_img_hash PROPERTIES  LABELS "Extra;opencv_img_hash;Accuracy" WORKING_DIRECTORY "/mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/build/test-reports/accuracy" _BACKTRACE_TRIPLES "/mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/opencv-4.x/cmake/OpenCVUtils.cmake;1795;add_test;/mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/opencv-4.x/cmake/OpenCVModule.cmake;1375;ocv_add_test_from_target;/mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/opencv-4.x/cmake/OpenCVModule.cmake;1133;ocv_add_accuracy_tests;/mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/opencv_contrib-4.x/modules/img_hash/CMakeLists.txt;3;ocv_define_module;/mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/opencv_contrib-4.x/modules/img_hash/CMakeLists.txt;0;")

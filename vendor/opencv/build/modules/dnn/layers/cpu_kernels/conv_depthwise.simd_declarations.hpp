#define CV_CPU_SIMD_FILENAME "/mnt/574cdeb8-9b3d-49a4-aa16-95589af03bdf/WorkingPickingPoint/vendor/opencv/opencv-4.x/modules/dnn/src/layers/cpu_kernels/conv_depthwise.simd.hpp"
#define CV_CPU_DISPATCH_MODE AVX
#include "opencv2/core/private/cv_cpu_include_simd_declarations.hpp"

#define CV_CPU_DISPATCH_MODE AVX2
#include "opencv2/core/private/cv_cpu_include_simd_declarations.hpp"

#define CV_CPU_DISPATCH_MODE RVV
#include "opencv2/core/private/cv_cpu_include_simd_declarations.hpp"

#define CV_CPU_DISPATCH_MODE LASX
#include "opencv2/core/private/cv_cpu_include_simd_declarations.hpp"

#define CV_CPU_DISPATCH_MODES_ALL LASX, RVV, AVX2, AVX, BASELINE

#undef CV_CPU_SIMD_FILENAME

#pragma once

#include "biscuit/config.h"

#pragma warning(push)
#pragma warning(disable: 4127 5054)

#include <opencv2/cvconfig.h>
#include <opencv2/opencv.hpp>
#ifdef HAVE_CUDA
#include <opencv2/cudawarping.hpp>
#endif

#pragma warning(pop)


/*
 * standart.hpp
 *
 *  Created on: 1 Sep 2011
 *      Author: valery
 */

#pragma once

#include "standard_base.hpp"

//==omp
#include <omp.h>

//==sys
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/syscall.h>

//==our
// utils
#include "cpp_utils.hpp"

// io
#include "io/ireader.hpp"
#include "io/converting_reader_wrapper.hpp"

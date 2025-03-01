// This file is part of libigl, a simple c++ geometry processing library.
// 
// Copyright (C) 2014 Daniele Panozzo <daniele.panozzo@gmail.com>
// 
// This Source Code Form is subject to the terms of the Mozilla Public License 
// v. 2.0. If a copy of the MPL was not distributed with this file, You can 
// obtain one at http://mozilla.org/MPL/2.0/.
#ifndef IGL_FILE_DIALOG_OPEN_H
#define IGL_FILE_DIALOG_OPEN_H
#include "igl_inline.h"

#include <string>

namespace igl
{
  // Returns a string with a path to an existing file
  // The string is returned empty if no file is selected
  // (on Linux machines, it assumes that Zenity is installed)
  //
  // Usage:
  //   std::string str = get_open_file_path();
  IGL_INLINE std::string file_dialog_open(const std::string& presetFileName="", const char* filter="All files (*.*)\0*.*\0");
}

#ifndef IGL_STATIC_LIBRARY
#  include "file_dialog_open.cpp"
#endif

#endif


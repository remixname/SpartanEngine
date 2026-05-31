/*
Copyright(c) 2015-2026 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "pch.h"
#include "Serialization.hpp"
#include <sstream>

namespace spartan
{
    const char* to_c_str(spartan::math::Matrix rhs)
    {
        std::stringstream ss;
        ss << rhs.m00 << " " << rhs.m01 << " " << rhs.m02 << " " << rhs.m03 << " "
            << rhs.m10 << " " << rhs.m11 << " " << rhs.m12 << " " << rhs.m13 << " "
            << rhs.m20 << " " << rhs.m21 << " " << rhs.m22 << " " << rhs.m23 << " "
            << rhs.m30 << " " << rhs.m31 << " " << rhs.m32 << " " << rhs.m33;
        return ss.str().c_str();
    }

}

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

#pragma once

#include <sstream>
#include "../Math/Matrix.h"
#include "../Logging/Log.h"
#include "pugixml.hpp"
namespace pugi
{
    namespace ext
    {
        spartan::math::Matrix get_value_matrix(const char* value);

        class xml_attribute
        {
        public:
            xml_attribute() = default;
            xml_attribute(pugi::xml_attribute a) : _attr(a) {}
            operator pugi::xml_attribute() const { return _attr; }

            spartan::math::Matrix as_matrix(spartan::math::Matrix def = spartan::math::Matrix()) const
            {
                if (!_attr) return def;
                const char_t* value = _attr.value();
                return value ? get_value_matrix(value) : def;
            }

        private:
            pugi::xml_attribute _attr;
        };

        spartan::math::Matrix get_value_matrix(const char* value)
        {
            std::stringstream ss(value);
            spartan::math::Matrix m;
            ss >> m.m00 >> m.m01 >> m.m02 >> m.m03
                >> m.m10 >> m.m11 >> m.m12 >> m.m13
                >> m.m20 >> m.m21 >> m.m22 >> m.m23
                >> m.m30 >> m.m31 >> m.m32 >> m.m33;
            if (!ss.fail())
            {
                return m;
            }
            else
            {
                SP_LOG_ERROR("failed to parse matrix from '%s'", value);
                return spartan::math::Matrix();
            }
        }
    }
}

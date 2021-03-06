// +-------------------------------------------------------------------------
// | quantization.cpp
// | 
// | Author: Gilbert Bernstein
// +-------------------------------------------------------------------------
// | COPYRIGHT:
// |    Copyright Gilbert Bernstein 2013
// |    See the included COPYRIGHT file for further details.
// |    
// |    This file is part of the Cork library.
// |
// |    Cork is free software: you can redistribute it and/or modify
// |    it under the terms of the GNU Lesser General Public License as
// |    published by the Free Software Foundation, either version 3 of
// |    the License, or (at your option) any later version.
// |
// |    Cork is distributed in the hope that it will be useful,
// |    but WITHOUT ANY WARRANTY; without even the implied warranty of
// |    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// |    GNU Lesser General Public License for more details.
// |
// |    You should have received a copy 
// |    of the GNU Lesser General Public License
// |    along with Cork.  If not, see <http://www.gnu.org/licenses/>.
// +-------------------------------------------------------------------------
#include "quantization.h"

namespace Quantization
{
	double MAGNIFY = 1.0;
	double RESHRINK = 1.0;

	float valueOne = 1.0;

#ifdef CORK_SSE
	__m128	MAGNIFY_SSE = _mm_load_ps1( &valueOne );
	__m128	RESHRINK_SSE = _mm_load_ps1( &valueOne );
#endif
} // end namespace Quantization


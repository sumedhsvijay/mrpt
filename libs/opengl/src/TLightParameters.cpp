/* +------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)            |
   |                          https://www.mrpt.org/                         |
   |                                                                        |
   | Copyright (c) 2005-2023, Individual contributors, see AUTHORS file     |
   | See: https://www.mrpt.org/Authors - All rights reserved.               |
   | Released under BSD License. See: https://www.mrpt.org/License          |
   +------------------------------------------------------------------------+ */

#include "opengl-precomp.h"	 // Precompiled header
//
#include <mrpt/opengl/TLightParameters.h>
#include <mrpt/serialization/CArchive.h>

using namespace mrpt;
using namespace mrpt::opengl;
using namespace std;

void TLightParameters::writeToStream(mrpt::serialization::CArchive& out) const
{
	const uint8_t version = 1;
	out << version;

	out << diffuse << ambient << specular << direction << color;
}

void TLightParameters::readFromStream(mrpt::serialization::CArchive& in)
{
	uint8_t version;
	in >> version;

	switch (version)
	{
		case 0:
		{
			mrpt::img::TColorf diffuseCol, ambientCol, specularCol;
			in >> diffuseCol >> ambientCol >> specularCol >> direction;
			ambient = ambientCol.R;
			specular = specularCol.R;
			diffuse = 1.0f;
			color = diffuseCol;
		}
		break;
		case 1:
			in >> diffuse >> ambient >> specular >> direction >> color;
			break;
		default: MRPT_THROW_UNKNOWN_SERIALIZATION_VERSION(version);
	};
}

namespace mrpt::opengl
{
mrpt::serialization::CArchive& operator>>(
	mrpt::serialization::CArchive& in, mrpt::opengl::TLightParameters& o)
{
	o.readFromStream(in);
	return in;
}
mrpt::serialization::CArchive& operator<<(
	mrpt::serialization::CArchive& out, const mrpt::opengl::TLightParameters& o)
{
	o.writeToStream(out);
	return out;
}
}  // namespace mrpt::opengl

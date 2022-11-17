#pragma once
#ifndef __sampling_h__
#define __sampling_h__

#include "Core/FeimosRender.h"
#include "Core/Geometry.hpp"
#include "Core/Transform.h"

namespace Feimos
{

    Point2f ConcentricSampleDisk(const Point2f &u);

}

#endif

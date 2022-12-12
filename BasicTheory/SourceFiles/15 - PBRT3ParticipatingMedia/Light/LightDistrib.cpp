#include "Light/LightDistrib.h"
#include "Sampler/Sampling.h"
#include <vector>
#include "Core/Scene.h"

namespace Feimos
{

	UniformLightDistribution::UniformLightDistribution(const Scene &scene)
	{
		std::vector<float> prob(scene.lights.size(), float(1));
		distrib.reset(new Distribution1D(&prob[0], int(prob.size())));
	}

	const Distribution1D *UniformLightDistribution::Lookup(const Point3f &p) const
	{
		return distrib.get();
	}

	std::unique_ptr<LightDistribution>
	CreateLightSampleDistribution(const std::string &name, const Scene &scene)
	{
		return std::unique_ptr<LightDistribution>{
			new UniformLightDistribution(scene)};
	}

}

#pragma once
#include "Media\Medium.h"
#include "Core\FeimosRender.h"
#include "Core\Transform.h"
#include "Core\Spectrum.h"

#include <iostream>
#include <fstream>
#include <string> 

namespace Feimos {

// GridDensityMedium Declarations
class GridDensityMedium : public Medium {
public:
	// GridDensityMedium Public Methods
	GridDensityMedium(const Spectrum &sigma_a, const Spectrum &sigma_s, float g,
		int nx, int ny, int nz, const Transform &mediumToWorld,
		const float *d)
		: sigma_a(sigma_a),
		sigma_s(sigma_s),
		g(g),
		nx(nx),
		ny(ny),
		nz(nz),
		WorldToMedium(Inverse(mediumToWorld)),
		density(new float[nx * ny * nz]) {
		memcpy((float *)density.get(), d, sizeof(float) * nx * ny * nz);
		// Precompute values for Monte Carlo sampling of _GridDensityMedium_
		sigma_t = (sigma_a + sigma_s)[0];
		if (Spectrum(sigma_t) != sigma_a + sigma_s) {
			//³ö´í
		}
		float maxDensity = 0;
		for (int i = 0; i < nx * ny * nz; ++i)
			maxDensity = std::max(maxDensity, density[i]);
		invMaxDensity = 1.f / maxDensity;
	}

	float Density(const Point3f &p) const;
	float D(const Point3i &p) const {
		Bounds3i sampleBounds(Point3i(0, 0, 0), Point3i(nx, ny, nz));
		if (!InsideExclusive(p, sampleBounds)) return 0;
		return density[(p.z * ny + p.y) * nx + p.x];
	}
	Spectrum Sample(const Ray &ray, Sampler &sampler, MediumInteraction *mi) const;
	Spectrum Tr(const Ray &ray, Sampler &sampler) const;
private:
	// GridDensityMedium Private Data
	const Spectrum sigma_a, sigma_s;
	const float g;
	const int nx, ny, nz;
	const Transform WorldToMedium;
	std::unique_ptr<float[]> density;
	float sigma_t;
	float invMaxDensity;
};

class MediumLoad {
public:
	MediumLoad(std::string name, const Transform &medium2world) {
		std::fstream f(name);
		std::string ed;
		f >> ed; f >> nx;
		f >> ed; f >> ny;
		f >> ed; f >> nz;
		Point3f p0; Point3f p1;
		f >> ed; f >> p0.x; f >> p0.y; f >> p0.z;
		f >> ed; f >> p1.x; f >> p1.y; f >> p1.z;
		
		// Ô¤¶¨Òå
		float sig_a_rgb[3], sig_s_rgb[3];
		f >> ed; f >> sig_a_rgb[0]; f >> sig_a_rgb[1]; f >> sig_a_rgb[2];
		f >> ed; f >> sig_s_rgb[0]; f >> sig_s_rgb[1]; f >> sig_s_rgb[2];

		Spectrum sig_a = Spectrum::FromRGB(sig_a_rgb), sig_s = 0.2 * Spectrum::FromRGB(sig_s_rgb);
		float g = -0.5f;

		float *data = new float[nx * ny * nz];
		for (int i = 0; i < nx*ny*nz; i++) {
			f >> data[i];
		}
		Transform data2Medium = Translate(Vector3f(p0)) *
			Scale(p1.x - p0.x, p1.y - p0.y, p1.z - p0.z);
		med = std::make_shared<GridDensityMedium>(sig_a, sig_s, g, nx, ny, nz,
			medium2world * data2Medium, data);
		f.close();
		delete[] data;
	}
	std::shared_ptr<Medium> med;
	int nx, ny, nz;

};



}








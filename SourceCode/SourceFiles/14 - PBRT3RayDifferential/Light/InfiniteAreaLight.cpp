#include "Light/InfiniteAreaLight.h"
#include "Sampler/Sampling.h"
#include "3rdLib/stb_image.h"
#include "Core/Geometry.h"

namespace Feimos
{

	// InfiniteAreaLight Method Definitions
	InfiniteAreaLight::InfiniteAreaLight(const Transform &LightToWorld,
										 const Spectrum &L, int nSamples,
										 const std::string &texmap)
		: Light((int)LightFlags::Infinite, LightToWorld, nSamples)
	{
		// Read texel data from _texmap_ and initialize _Lmap_
		Point2i resolution;

		float *data;
		RGBSpectrum *data_s;
		if (texmap != "")
		{
			// texels = ReadImage(texmap, &resolution);
			int imageWidth, imageHeight, nrComponents;
			data = stbi_loadf(texmap.c_str(), &imageWidth, &imageHeight, &nrComponents, 0);
			data_s = new RGBSpectrum[imageWidth * imageHeight];
			for (int j = 0; j < imageHeight; j++)
			{
				for (int i = 0; i < imageWidth; i++)
				{
					RGBSpectrum r;
					r[0] = data[(i + j * imageWidth) * nrComponents + 0];
					r[0] = L[0] * r[0]; // *r[0];
					r[1] = data[(i + j * imageWidth) * nrComponents + 1];
					r[1] = L[1] * r[1]; // *r[1];
					r[2] = data[(i + j * imageWidth) * nrComponents + 2];
					r[2] = L[2] * r[2]; // *r[2];
					data_s[i + j * imageWidth] = r * Sqrt(r);
				}
			}
			resolution.x = imageWidth;
			resolution.y = imageHeight;
		}
		free(data);
		std::unique_ptr<RGBSpectrum[]> texels(data_s);

		if (!texels)
		{
			resolution.x = resolution.y = 1;
			texels = std::unique_ptr<RGBSpectrum[]>(new RGBSpectrum[1]);
			texels[0] = L.ToRGBSpectrum();
		}
		Lmap.reset(new MIPMap<RGBSpectrum>(resolution, texels.get()));

		// Initialize sampling PDFs for infinite area light

		// Compute scalar-valued image _img_ from environment map
		int width = 2 * Lmap->Width(), height = 2 * Lmap->Height();
		std::unique_ptr<float[]> img(new float[width * height]);
		float fwidth = 0.5f / std::min(width, height);
		for (int v = 0; v < height; v++)
		{
			float vp = (v + .5f) / (float)height;
			float sinTheta = std::sin(Pi * (v + .5f) / height);
			for (int u = 0; u < width; ++u)
			{
				float up = (u + .5f) / (float)width;
				img[u + v * width] = Lmap->Lookup(Point2f(up, vp), fwidth).y();
				img[u + v * width] *= sinTheta;
			}
		}
		// Compute sampling distributions for rows and columns of image
		distribution.reset(new Distribution2D(img.get(), width, height));
	}
	Spectrum InfiniteAreaLight::Power() const
	{
		return Pi * worldRadius * worldRadius *
			   Spectrum(Lmap->Lookup(Point2f(.5f, .5f), .5f),
						SpectrumType::Illuminant);
	}
	Spectrum InfiniteAreaLight::Le(const RayDifferential &ray) const
	{
		Vector3f w = Normalize(WorldToLight(ray.d));
		// std::acos(v.z) [0,pi]
		Point2f st(SphericalPhi(w) * Inv2Pi, SphericalTheta(w) * InvPi);
		return Spectrum(Lmap->Lookup(st), SpectrumType::Illuminant);
	}
	Spectrum InfiniteAreaLight::Sample_Li(const Interaction &ref, const Point2f &u,
										  Vector3f *wi, float *pdf,
										  VisibilityTester *vis) const
	{
		// Find $(u,v)$ sample coordinates in infinite light texture
		float mapPdf;
		Point2f uv = distribution->SampleContinuous(u, &mapPdf);
		if (mapPdf == 0)
			return Spectrum(0.f);

		// Convert infinite light sample point to direction
		float theta = uv[1] * Pi, phi = uv[0] * 2 * Pi;
		float cosTheta = std::cos(theta), sinTheta = std::sin(theta);
		float sinPhi = std::sin(phi), cosPhi = std::cos(phi);
		*wi =
			LightToWorld(Vector3f(sinTheta * cosPhi, sinTheta * sinPhi, cosTheta));

		// Compute PDF for sampled infinite light direction
		*pdf = mapPdf / (2 * Pi * Pi * sinTheta);
		if (sinTheta == 0)
			*pdf = 0;

		// Return radiance value for infinite light direction
		*vis = VisibilityTester(ref, Interaction(ref.p + *wi * (2 * worldRadius), ref.time));
		return 2 * Spectrum(Lmap->Lookup(uv), SpectrumType::Illuminant);
	}
	float InfiniteAreaLight::Pdf_Li(const Interaction &, const Vector3f &w) const
	{
		Vector3f wi = WorldToLight(w);
		float theta = SphericalTheta(wi), phi = SphericalPhi(wi);
		float sinTheta = std::sin(theta);
		if (sinTheta == 0)
			return 0;
		return distribution->Pdf(Point2f(phi * Inv2Pi, theta * InvPi)) /
			   (2 * Pi * Pi * sinTheta);
	}

}

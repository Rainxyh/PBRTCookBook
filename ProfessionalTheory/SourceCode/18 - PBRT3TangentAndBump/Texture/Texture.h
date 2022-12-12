#pragma once
#ifndef __TEXTURE_H__
#define __TEXTURE_H__
#include "Core/Geometry.h"
#include "Core/FeimosRender.h"

namespace Feimos
{

	// Texture Declarations
	class TextureMapping2D
	{
	public:
		// TextureMapping2D Interface
		virtual ~TextureMapping2D() {}
		virtual Point2f Map(const SurfaceInteraction &si, Vector2f *dstdx,
							Vector2f *dstdy) const = 0;
	};

	class UVMapping2D : public TextureMapping2D
	{
	public:
		// UVMapping2D Public Methods
		UVMapping2D(float su = 1, float sv = 1, float du = 0, float dv = 0)
			: su(su), sv(sv), du(du), dv(dv) {}
		Point2f Map(const SurfaceInteraction &si, Vector2f *dstdx,
					Vector2f *dstdy) const;

	private:
		const float su, sv, du, dv;
	};

	template <typename T>
	class Texture
	{
	public:
		// Texture Interface
		virtual T Evaluate(const SurfaceInteraction &) const = 0;
		virtual ~Texture() {}
	};

	float Lanczos(float, float tau = 2);

}

#endif

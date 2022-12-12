#include "ImageProcess.h"

#ifdef _WIN32
#define windows_operating_system true
#elif __linux__
#define windows_operating_system false
#elif __APPLE__
#define windows_operating_system false
#endif

#if windows_operating_system
#include <windows.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "3rdLib/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "3rdLib/stb_image_write.h"
namespace Rattler
{

	bool imageCopy(std::string from, std::string to)
	{

		std::string toDir = to.substr(0, to.find_last_of('/'));
#if windows_operating_system
		CreateDirectory(toDir.c_str(), NULL);
#else
		int warn_unused_result = system(("mkdir " + toDir).c_str());
#endif

		int width, height, nrChannels;
		unsigned char *data = stbi_load(from.c_str(), &width, &height, &nrChannels, 0);

		if (nrChannels == 4)
		{
			stbi_write_png(to.c_str(), width, height, nrChannels, data, nrChannels * width * sizeof(unsigned char));
		}
		else
		{

			if (nrChannels < 3 || nrChannels > 4)
			{
				delete[] data;
				return false;
			}

			unsigned char *alpha = new unsigned char[width * height * 4];
#pragma omp parallel for
			for (int i = 0; i < width; i++)
			{
				for (int j = 0; j < height; j++)
				{
					int offset = (i + j * width);

					alpha[offset * 4 + 0] = data[offset * 3 + 0];
					alpha[offset * 4 + 1] = data[offset * 3 + 1];
					alpha[offset * 4 + 2] = data[offset * 3 + 2];
					alpha[offset * 4 + 3] = 255;
				}
			}
			stbi_write_png(to.c_str(), width, height, 4, alpha, 4 * width * sizeof(unsigned char));
			delete[] alpha;
		}

		delete[] data;
		return true;
	}

#include <omp.h>
	bool abstractAlpha(std::string from, std::string to)
	{
		std::string toDir = to.substr(0, to.find_last_of('/'));

#if windows_operating_system
		CreateDirectory(toDir.c_str(), NULL);
#else
		int warn_unused_result = system(("mkdir " + toDir).c_str());
#endif

		int width, height, nrChannels;
		unsigned char *data = stbi_load(from.c_str(), &width, &height, &nrChannels, 0);
		if (nrChannels != 4)
		{
			delete[] data;
			return false;
		}
		unsigned char *alpha = new unsigned char[width * height * nrChannels];

#pragma omp parallel for
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				int offset = (i + j * width) * nrChannels;
				if (data[offset + 3] == 0)
				{
					alpha[offset + 0] = 0;
					alpha[offset + 1] = 0;
					alpha[offset + 2] = 0;
					alpha[offset + 3] = 0;
				}
				else
				{
					alpha[offset + 0] = 255;
					alpha[offset + 1] = 255;
					alpha[offset + 2] = 255;
					alpha[offset + 3] = 255;
				}
			}
		}

		stbi_write_png(to.c_str(), width, height, nrChannels, alpha, nrChannels * width * sizeof(unsigned char));

		delete[] data;
		delete[] alpha;
		return true;
	}

}

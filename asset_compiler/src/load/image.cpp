#include "image.h"

#include "asset/image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void loadImage(AssetPackage* package, const std::string& path, const std::string& filepath) {
	int width, height, channels, format;
	stbi_info(filepath.c_str(), &width, &height, &channels);

	stbi_set_flip_vertically_on_load(true);

	switch (channels) {
        case 1: format = STBI_grey;       break;
        case 2: format = STBI_grey_alpha; break;
        case 3: format = STBI_rgb;        break;
        case 4: format = STBI_rgb_alpha;  break;
        default:
            throw std::runtime_error("Failed to load image, invalid number of channels");
	}

	char* pixels = (char*)stbi_load(filepath.c_str(), &width, &height, &channels, format);

	if (!pixels) {
        throw std::runtime_error("Failed to load image, no pixels");
	}

    size_t size = width * height * channels;

    ImageAsset image;
    image.pixels = std::vector<char>(pixels, pixels + size);
    image.width = width;
    image.height = height;
    image.channels = channels;
    image.hasFlippedY = true;

    package->add(image.createAsset(path));
}
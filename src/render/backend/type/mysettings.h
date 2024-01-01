#pragma once

enum ImageFormat {
    ImageFormat_RGB_8_SRGB,
    ImageFormat_RGB_8_UNORM,
    ImageFormat_RGBA_8_SRGB,
    ImageFormat_RGBA_8_UNORM,
};

enum ImageSampleCount {
    ImageSampleCount_1,
    ImageSampleCount_2,
    ImageSampleCount_4,
    ImageSampleCount_8,
    ImageSampleCount_16,
    ImageSampleCount_32,
    ImageSampleCount_64,
};

enum BufferType {
    BufferType_TransferSource,
    BufferType_Vertex,
    BufferType_Index,
    BufferType_Uniform,
};
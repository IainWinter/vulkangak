cd "${0%/*}"

mkdir ./build/shaders

glslc ./src_assets/shaders/particle/particle.vert -o ./build/shaders/particle_vert.spv
glslc ./src_assets/shaders/particle/particle.frag -o ./build/shaders/particle_frag.spv

glslc ./src_assets/shaders/line/line.vert -o ./build/shaders/line_vert.spv
glslc ./src_assets/shaders/line/line.frag -o ./build/shaders/line_frag.spv

glslc ./src_assets/shaders/text_msdf/text_msdf.vert -o ./build/shaders/text_msdf_vert.spv
glslc ./src_assets/shaders/text_msdf/text_msdf.frag -o ./build/shaders/text_msdf_frag.spv

glslc ./src_assets/shaders/sprite/sprite.vert -o ./build/shaders/sprite_vert.spv
glslc ./src_assets/shaders/sprite/sprite.frag -o ./build/shaders/sprite_frag.spv

./asset_compiler/build/lithAssetCompiler ./assets.input ./build/assets.bin
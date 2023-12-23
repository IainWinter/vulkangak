cd "${0%/*}"

mkdir ./build/shaders
glslc ./src_assets/shaders/triangle.frag -o ./build/shaders/frag.spv
glslc ./src_assets/shaders/triangle.vert -o ./build/shaders/vert.spv

./asset_compiler/build/lithAssetCompiler ./assets.input ./build/assets.bin
vert ./build/shaders/vert.spv
frag ./build/shaders/frag.spv

push vert 64

descriptor vert uniform_buffer 1 0 1
descriptor frag image 1 0 0

test.shader
./src_assets/test.shader
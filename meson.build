project('vkTestCpp', 'cpp', default_options: ['cpp_std=c++20'])

sources = [
    'src/main.cpp',

    'src/window.cpp',
    'src/simulation_loop.cpp',
    'src/input.cpp',

    'src/math/math.cpp',
    'src/math/random.cpp',

    'src/render/backend/render_device.cpp',
    'src/render/backend/descriptor_group.cpp',
    'src/render/backend/command_buffer.cpp',
    'src/render/backend/shader.cpp',
    'src/render/backend/image.cpp',
    'src/render/backend/buffer.cpp',
    'src/render/backend/vertex_buffer.cpp',
    'src/render/backend/index_buffer.cpp',
    'src/render/backend/uniform_buffer.cpp',
    'src/render/backend/imgui_loop.cpp',

    'src/render/line/line_mesh.cpp',

    'src/render/lens.cpp',
]

include_dirs = [
    'src',
    'asset_compiler/include'
]

deps = [
	dependency('glm'),
	dependency('sdl2', default_options: { 'test': false }),
	dependency('vulkan'),
    dependency('imgui')
]

args = ['-DSDL_MAIN_HANDLED=1']

executable('vkTestCpp', sources, include_directories: include_dirs, dependencies: deps, cpp_args: args)
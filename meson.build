project('vkTestCpp', 'cpp', default_options: ['cpp_std=c++17'])

sources = [
    'src/window.cpp',
    'src/main.cpp',
    'src/simulation_loop.cpp',

    'src/render/backend/render_device.cpp',
    'src/render/backend/descriptor_group.cpp',
    'src/render/backend/command_buffer.cpp',
    'src/render/backend/shader.cpp',
    'src/render/backend/image.cpp',
    'src/render/backend/buffer.cpp',
    'src/render/backend/vertex_buffer.cpp',
    'src/render/backend/index_buffer.cpp',
    'src/render/backend/uniform_buffer.cpp',
]

include_dirs = [
    'src'
]

deps = [
	dependency('glm'),
	dependency('sdl2', default_options: { 'test': false }),
	dependency('vulkan')
]

args = ['-DSDL_MAIN_HANDLED=1']

executable('vkTestCpp', sources, include_directories: include_dirs, dependencies: deps, cpp_args: args)
project('vkTestCpp', 'cpp', default_options: ['cpp_std=c++17'])

sources = [
    'src/window.cpp',
    'src/main.cpp',
    'src/simulation_loop.cpp',

    'src/render/render_device.cpp',
    'src/render/descriptor_group.cpp',
    'src/render/command_buffer.cpp',
    'src/render/shader.cpp',
    'src/render/image.cpp',
    'src/render/buffer.cpp',
    'src/render/vertex_buffer.cpp',
    'src/render/index_buffer.cpp',
    'src/render/uniform_buffer.cpp',
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
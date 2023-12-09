project('vkTestCpp', 'cpp', default_options: ['cpp_std=c++17'])

sources = [
    'window.cpp',
    'simulation_loop.cpp',
    'render_device.cpp',
    'descriptor_group.cpp',
    'shader.cpp',
    'buffer.cpp',
    'vertex_buffer.cpp',
    'index_buffer.cpp',
    'uniform_buffer.cpp',
    'command_buffer.cpp',
    'image.cpp',
    'main.cpp'
]

deps = [
	dependency('glm'),
	dependency('sdl2', default_options: { 'test': false }),
	dependency('vulkan')
]

args = ['-DSDL_MAIN_HANDLED=1']

executable('vkTestCpp', sources, dependencies: deps, cpp_args: args)
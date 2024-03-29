project('vkTestCpp', 'cpp', default_options: ['cpp_std=c++20'])

sources = [
    'src/main.cpp',

    'src/containers/defer_delete.cpp',

    'src/game/game_loop.cpp',
    'src/game/simulation_loop.cpp',
    'src/game/input.cpp',

    'src/math/math.cpp',
    'src/math/random.cpp',

    'src/render/lens.cpp',

    'src/render/line/line_mesh.cpp',
    'src/render/particle/particle_mesh.cpp',
    'src/render/sprite/sprite_mesh.cpp',
    'src/render/text/text_mesh.cpp',

    'src/render/backend/factory/platform/buffer_factory_vulkan.cpp',
    'src/render/backend/factory/platform/command_buffer_factory_vulkan.cpp',
    'src/render/backend/factory/platform/descriptor_set_factory_vulkan.cpp',
    'src/render/backend/factory/platform/descriptor_set_layout_factory_vulkan.cpp',
    'src/render/backend/factory/platform/frame_buffer_factory_vulkan.cpp',
    'src/render/backend/factory/platform/image_factory_vulkan.cpp',
    'src/render/backend/factory/platform/image_sampler_factory_vulkan.cpp',
    'src/render/backend/factory/platform/render_pass_factory_vulkan.cpp',
    'src/render/backend/factory/platform/shader_factory_vulkan.cpp',

    'src/render/backend/type/platform/buffer_vulkan.cpp',
    'src/render/backend/type/platform/command_buffer_vulkan.cpp',
    'src/render/backend/type/platform/descriptor_set_vulkan.cpp',
    'src/render/backend/type/platform/translation_vulkan.cpp',

    'src/render/backend/window.cpp',
    'src/render/backend/render_device.cpp',
    'src/render/backend/imgui_loop.cpp',
]

include_dirs = [
    'src',
    'asset_compiler/include'
]

deps = [
	dependency('fmt'),
	dependency('glm'),
	dependency('sdl2', default_options: { 'test': false }),
    dependency('imgui'),
	dependency('vulkan'),
	dependency('vulkan-memory-allocator'),
]

args = [
    # Use my own main function
    '-DSDL_MAIN_HANDLED=1', 

    # Vulkan needs the clip space to be [0, 1]
    '-DGLM_FORCE_DEPTH_ZERO_TO_ONE=1'
]

executable('vkTestCpp', sources, include_directories: include_dirs, dependencies: deps, cpp_args: args)
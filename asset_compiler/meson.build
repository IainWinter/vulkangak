project('lithAssetCompiler', 'cpp', default_options: ['cpp_std=c++17'])

subdir('subprojects_src/msdf-atlas-gen')

sources = [
    'src/main.cpp', 
    'src/package_input.cpp',
    
    'src/load/font.cpp',
    'src/load/image.cpp',
    'src/load/shader_bytecode.cpp',
]

include_dirs = [
    'include',
    'src'
]

deps = [
	msdf_atlas_gen_dep,
	msdfgen_dep,
    
    dependency('glm'),
]

args = msdf_atlas_args

executable('lithAssetCompiler', sources, include_directories: include_dirs, dependencies: deps, cpp_args: args)
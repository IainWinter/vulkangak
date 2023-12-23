project('lithAssetCompiler', 'cpp')

subdir('subprojects_src/msdf-atlas-gen')

sources = [
    'src/main.cpp', 
    'src/package_input.cpp',
    
    'src/asset_types.cpp',

    'src/load/font.cpp',
    'src/load/image.cpp',
    'src/load/shader_bytecode.cpp',
]

include_dirs = [
    '.',
    'src'
]

deps = [
	msdf_atlas_gen_dep,
	msdfgen_dep,
    
    dependency('glm'),
]

args = msdf_atlas_args

executable('lithAssetCompiler', sources, include_directories: include_dirs, dependencies: deps, cpp_args: args)
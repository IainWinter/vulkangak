import os
import subprocess

def run_command(command):
    subprocess.run(command, shell=True, check=True)

def compile_shader(input_file, output_directory):
    input_basename = os.path.splitext(os.path.basename(input_file))[0]
    output_filename = f"{input_basename}.spv"
    output_file = os.path.join(output_directory, output_filename)
    run_command(f'glslc {input_file} -o {output_file}')

# Change the current working directory to the script's directory
script_directory = os.path.dirname(os.path.abspath(__file__))
os.chdir(script_directory)

# Create directories
os.makedirs('./build/shaders', exist_ok=True)

# List of input shader files
shader_inputs = [
    './src_assets/shaders/particle/particle.vert',
    './src_assets/shaders/particle/particle.frag',
    './src_assets/shaders/line/line.vert',
    './src_assets/shaders/line/line.frag',
]

# Compile shaders
for input_file in shader_inputs:
    compile_shader(input_file, './build/shaders')

# Run the asset compiler
output_directory = './build'
output_filename = 'assets.bin'
output_file = os.path.join(output_directory, output_filename)
run_command(f'./asset_compiler/build/lithAssetCompiler.exe ./assets.input {output_file}')


shader_source = [
    './src_assets/shaders/particle/particle.vert',
    './src_assets/shaders/particle/particle.frag',
    './src_assets/shaders/line/line.vert',
    './src_assets/shaders/line/line.frag',
]

compile_shaders(shader_source, './build/shaders')



compile_assets()

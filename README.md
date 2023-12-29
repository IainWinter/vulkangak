# vulkangak

testing

## Assets

Static assets get compiled into a binary format called an Asset Package.

### Input Scheme
```
AssetType:
    image       -> stb_image loader
    vert | frag -> loads spv-5 shader bytecode

AssetInput:
    <asset_name: string>.<type: AssetType>
    <input_file: string>

<AssetInput>
<newline>
```

### Binary Scheme

#### Basic types
```
boolean: <1 byte>
number: <8 bytes>
float: <4 bytes>
newline: <\n>
```

#### Complex types
```
float2: <float> <float>
list<T>: <n: number> <n T>
binary: <list<char>>
string: <binary>
```

#### Asset Package
```
Asset:
    <name: string>
    <data: binary>

<assets: list<Asset>>
```

#### Image
```
<width: nunber> 
<height: number> 
<channels: number> 
<flipY: boolean>
<pixels: width * height * channels bytes>
```

#### Shader Bytecode
```
<binary>
```

#### Font
```
Glyph: 
    <posMin: float2>
    <posMax: float2>
    <uvMin: float2>
    <uvMax: float2>
    <advance: float>
    <index: int>
    <character: int>

Kerning:
    <left: int>
    <right: int>
    <amount: float>

<asset_path: string>
<glyphs: list<Glyph>>
<kerning: list<Kerning>>
<spaceAdvance: float>
<lineHeight: float>
<topHeight: float>
<bottomHeight: float>
```

## Input Mapper

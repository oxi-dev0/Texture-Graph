# Texture Graph Nodes

These are the core nodes for the Texture Graph app. They are written in the Texture Graph Node Language, and are loaded by the Texture Graph app to be compiled and evaluated at Runtime.

# TGNode Language v0.9 Documentation
> This language is not smart and requires specific spacing to work;   
> Each token must be seperated by a space, and each keyword must be separated by a newline.

A VSCode syntax extension can be found [here](https://github.com/oxi-dev0/TGNL)

**Index**
- [Types](#types)
- [Overview](#overview)
  - [Metadata Definition](#metadata-definition)
  - [Variable Definition](#variable-definition)
  - [Execution Definition](#execution-definition)
- [Core Functions](#core-functions)
- [Examples](#examples)

## TGNL Types
| Type | Definition | Example | Can be parameter? | Can be pin? |
| --- | --- | --- | --- | --- |
|`color` | A `0x` prefixed 8 char hex number that defines an 8-bit RGBA color | `0xFF0000FF` | &check; | &cross; |
|`int` | A single line integer | `324` | &check; | &cross; |
|`float` | A single line float that includes a decimal point | `10.0` | &check; | &cross; |
|`bool` | Either `true` or `false` | | &check; | &cross; |
|`vec2` | A 2D Vector | `{x=5,y=10}` | &check; | &cross; |
|`colortex` | A 2D Array of `color`s | | &cross; | &check; |
|`greytex` | A 2D Array of 8-bit greyscale `int`s | | &cross; | &check; |
> An `enum` type will be coming later in development

## Overview
The `.tgnode` file needs 3 blocks of definitions to work;
- `metadata`
- `variables`
- `execution`

## Metadata Definition
### Metadata Block
The metadata block defines data for the node's display in the app, such as the name, color and display names of variables.  
The `metadata` keyword must be placed before and after the node metadata.

### Metadata Keywords
| Keyword & Args | Definition |
| --- | --- |
| `name [string]` | Defines the name of the Node |
| `color [color]` | Defines the color of the Node |
| `category [string]` | Defines the category of the Node |
| | |
| `varname [luavar] [string]` | Defines the display name of a lua var. This is required for each lua var |
| `default [luavar] [data]` | Defines the default value of a parameter lua var. This is not required, but is only valid for parameter vars. `[data]`'s type depends on the variable's type |
| | |
| `display [luavar]` | Defines the output lua var that should be used as the node preview. This is only valid for lua vars with type `greytex`, or `colortex` |
| | |
| `show [luavar] [condition]` | Allows parameter lua vars to only be shown if one of the other parameter vars meet a condition. If a variable is not shown, it will be set to it's default value. Parameters being dynamically shown must have a default value declared |
> `show` is not implemented currently, as I will be working on conditions later. It will most likely be worked on when I add the `enum` type.  

## Variable Definition
> Var Definitions must be placed after the `metadata` block, and before the `exec` block. This block does not require a keyword to define where it is.

### Variable Definition Keywords
| Keyword & Args | Definition |
| --- | --- |
| `param [type] [luavar]` | Defines a parameter lua var. Parameters are not pins, and are configurable in the inspector when selecting the node |
| `input [type] [luavar]` | Defines an input lua var. Inputs are pins and can not have default values.
| `output [type] [luavar]` | Defines an output lua var. Outputs are pins and can not have default values.

## Execution Definition
> Nodes calculate their values through lua code that is ran on the texture generation thread.

### Syntax
```lua
exec
{
  [LUA CODE]
}
```
> This exact syntax must be met, and placed at the end of the `.tgnode` file. It is to ensure that the lua code is distinctly defined away from the rest of the code.

### Lua Vars
Before the execution lua is ran, the Node Variables get defined to allow the lua code to interface with the node's data.  
- `param` lua vars get defined with a value of what is set in the Node's inspector.  
- `input` lua vars get defined with the value that is connected to it's pin.  
- `output` lua vars get defined with the type's default. At the end of execution, it's value will be read to determine what it outputs on it's pin.

TGNL types are converted to lua types to allow them to be processed
| TGNL Type | Lua Type | Details |
| --- | --- | --- |
| `float` | `float` | |
| `int` | `int` | |
| `bool` | `bool` | |
| `vec2` | `table` | Vector2s are formatted like this: `{x=0, y=0}`. |
| `color` | `table` | Colors are formatted like this: `{r=255, g=0, b=0, a=255}`. |
| `colortex` | `2D Table of Color(table)` | An example of getting the red channel of the pixel color at (25,25) would be `Texture[25][25].r`. |
| `greytex` | `2D Table of int(table)` | An example of getting the pixel value at (25,25) would be `Texture[25][25]`. |

### Other Vars
Again, before the execution lua is ran, these variables get defined to allow the lua code to interface with the node's data.  
These variables are extra and do not need to be defined in the `.tgnode` file.
| Variable | Type | Description |
| --- | --- | --- |
| `sizeX` | `int` | The graph texture width in pixels |
| `sizeY` | `int` | The graph texture height in pixels |

## Core Functions
A node's execution lua has core functions available to use, in a similar style to glsl. These core functions are defined in `library/Nodes/core.lua`, and are compiled into the execution lua for each node when the application starts, or when `Engine > Recompile Node Library` is called.

### Function Definitions
| Function | Parameter Types | Return Type | Description |
| --- | --- | --- | --- |
| | | | |
| **Vector Functions** |
| `vec2(x,y)` | `x:number`, `y:number` | `vec2` | Returns a Vector2 with the specified components. |
| `addV(a,b)` | `a:vec2`, `b:vec2` | `vec2` | Returns a + b |
| `addVC(a,b)` | `a:vec2`, `b:number` | `vec2` | Returns a + b |
| `subV(a,b)` | `a:vec2`, `b:vec2` | `vec2` | Returns a - b |
| `subVC(a,b)` | `a:vec2`, `b:number` | `vec2` | Returns a - b |
| `mulV(a,b)` | `a:vec2`, `b:vec2` | `vec2` | Returns a * b |
| `mulVC(a,b)` | `a:vec2`, `b:number` | `vec2` | Returns a * b |
| `divV(a,b)` | `a:vec2`, `b:vec2` | `vec2` | Returns a * b |
| `divVC(a,b)` | `a:vec2`, `b:number` | `vec2` | Returns a * b |
| `negateV(v)` | `v:vec2` | `vec2` | Returns -v |
| `lengthV(v)` | `v:vec2` | `number` | Returns the Sqrt Length of v |
| `normalize(v)` | `v:vec2` | `vec2` | Returns v divided by its length |
| `dotV(a,b)` | `a:vec2`, `b:vec2` | `number` | Returns the dot product of a and b |
| `crossV(a,b)` | `a:vec2`, `b:vec2` | `number` | Returns the cross product of a and b |
| | | |
| **Misc. Math Functions** |
| `clamp(v,mi,ma)` | `v:number`, `mi:number`, 'ma:number' | `number` | Returns a so that mi <= a <= ma |
| `step(edge,x)` | `edge:number`, `x:number` | `number` | Returns 0 if x < edge, returns 1 otherwise |
| `mix(a,b,v)` | `a:number`, `b:number`, `v:number` | `number` | Returns a linear interpolation between a and b, using v as the weight |
| `smoothstep(edge0,edge1,x)` | `edge0:number`, `edge1:number`, `x:number` | `number` | Returns smooth Hermite interpolation between 0 and 1 when edge0 < x < edge1 |
| `fract(x)` | `x:number` | `number` | Returns the fractional part of x (computed by x - floor(x)) |
| | | |
| **2-Dim Misc. Math Functions** |
| `floorV(v)` | `v:vec2` | `vec2` | Returns a vec2 where each component has been floored |
| `fractV(v)` | `v:vec2` | `vec2` | Returns a vec2 where each component is it's fractional part |
| `smoothstepVC(edge0,edge1,v)` | `edge0:number`, `edge1:number`, `v:vec2` | `vec2` | Returns, for each component of the vector (c), smooth Hermite interpolation of c between 0 and 1 when edge0 < c < edge1 |

> More functions will be added in the future.

## Examples
### Example 1 (Simple)
> Ignore the markdown syntax highlighting for the metadata and variable definitions.
```lua
metadata
    name "Solid Colour"
    color 0x8AE9C1FF

    varname inColor "Colour"
    varname out "Out"

    default Colour 0xFF0000FF

    display Out
metadata

param color inColor
output colortex out

exec
{
    for x=1, sizeX do
        for y=1, sizeY do
            out[x,y] = inColor
        end
    end
}
```
> Constans/SolidColor.tgnode

#### Metadata
This metadata sets the Node's name to be 'Simple Colour', and gives it the colour 0x8AE9C1FF, which is RGBA(138, 233, 193, 255). It then defines two display names for the variables, `inColor:"Colour"` and `out:"Out"`. It sets `inColor`'s default value to 0xFF0000FF, which is RGBA(255, 0, 0, 255), and then specifies the variable `out` to be the display variable for the Node. (It will show up on the Node in the graph).

#### Variables
It then defines the `inColor` variable to be of the `color` type, which is a table in the format of `{r=0,g=0,b=0,a=0}`, and defines the `out` variable to be of the `colortex` type, which is a 2D array of `color`s.

#### Execution
This lua loops through every pixel in the texture, setting the pixel in `out` to be the parameter variable `inColor`, which is of type `color`. This will result in a texture with a solid color, that can be changed in the inspector.

### Example 2 (Complex)
> Ignore the markdown syntax highlighting for the metadata and variable definitions.
```lua
metadata
    name "Gradient"
    color 0x9934EBFF
    category "Core/Patterns"

    varname dir "Direction"
    varname Out "Out"

    default dir 1.0,0.0

    display Out
metadata

param vec2 dir
output greytex Out

exec
{
    local norm = normalizeV(dir)
    local bb = dotV(norm,norm)
    local max = lengthV(vec2(sizeX*norm.x,sizeY*norm.y))

    for x=1, sizeX do
        for y=1, sizeY do
            local vec = vec2(x,y)
            local proj = mulVC(norm, dotV(vec, norm) / bb)
            local d = lengthV(vec2(proj.x,proj.y))
            local col = math.floor(clamp(d/max,0,1)*255)
            Out[x][y] = col
        end
    end
}
```
> Patterns/Gradient.tgnode

#### Metadata
This metadata sets the Node's name to be 'Gradient', and gives it the colour 0x9934EBFF, which is RGBA(153, 52, 235, 255). It then defines two display names for the variables, `dir:"Direction"` and `out:"Out"`. It sets `dir`'s default value to `vec2(1.0, 0.0)`, and then specifies the variable `out` to be the display variable for the Node. (It will show up on the Node in the graph).

#### Variables
It then defines the `dir` variable to be of the `vec2` type, which is a table in the format of `{x=0,y=0}`, and defines the `out` variable to be of the `greytex` type, which is a 2D array of `int`s.

#### Execution
This lua first uses the Core Functions to calculate the normalized form of the `dir` variable, and calculates the dot product of itself. it also calculates the maximum length that the distance a pixel could be, from the top left. It then loops through each pixel of the texture, and, using the Core Functions to manipulate the vectors, calculates the end greyscale value (`col`) to be from 0 to 255. It then sets this pixel in the `out` texture to be this greyscale value.

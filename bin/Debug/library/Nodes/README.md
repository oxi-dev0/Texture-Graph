# Texture Graph Nodes

These are the core nodes for the Texture Graph app. They are written in the Texture Graph Node Language, and are loaded by the Texture Graph app to be compiled and evaluated at Runtime.

# TGNode Language v1.0 Documentation
> This language is not smart and requires specific spacing to work;   
> Each token must be seperated by a space, and each keyword must be separated by a newline.

A VSCode syntax extension can be found [here](https://github.com/oxi-dev0/TGNL)

**Index**
- [Types](#tgnl-types)
- [Overview](#overview)
  - [Metadata Definition](#metadata-definition)
  - [Program Definition](#program-definition)
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
|`enum`| A user-defined set of named options for a variable | `"Add","Subtract"` | &check; | &cross; |

## Overview
A Node requires a `.metadata` and a `program.lua` file to work.

## Metadata Definition
### .metadata file
The .metadata file defines data for the node's display in the app, such as the name, color and display names of variables.  

### Metadata Keywords
| Keyword & Args | Definition |
| --- | --- |
| `name [string]` | Defines the name of the Node |
| `color [color]` | Defines the color of the Node |
| `category [string]` | Defines the category of the Node |
| | |
| `param [type] [varname] [displayname] [default?] [enumdef*]` | Defines a parameter lua var. Parameters are not pins, and are configurable in the inspector when selecting the node |
| `input [type] [luavar]` | Defines an input lua var. Inputs are pins and can not have default values.
| `output [type] [luavar]` | Defines an output lua var. Outputs are pins and can not have default values.
| | |
| `display [luavar]` | Defines the output lua var that should be used as the node preview. This is only valid for lua vars with type `greytex`, or `colortex` |
| | |
| `show [luavar] [condition]` | Allows parameter lua vars to only be shown in the inspector depending on a condition. If a parameter is hidden, the program will use it's default value |

### Param keyword
The format for the param keyword is `param [type] [varname] [displayname] [default?] [enumdef*]`.  
`type` - The variable's type.  
`varname` - The name the variable will use in the lua program.  
`displayname` - A string that will be used in the inspector to name the variable.  
`default?` - The default value. (OPTIONAL).  
`enumdef*` - A comma-seperated list of strings with no spaces or brackets which define the enum options. E.G `"Add","Subtract","Divide","Multiply"`. (REQUIRED FOR ENUMS) 

## Program Definition
### program.lua file
> Nodes calculate their values through lua code that is ran on the texture generation thread.

### Variables
Before the program is ran, the node variables are defined in the lua state to allow the Node's lua program to interface with the Node's data.  
- `param` lua vars get defined with what is set in the Node's inspector.  
- `input` lua vars get defined with the value of what is connected to it's pin.  
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
| `divV(a,b)` | `a:vec2`, `b:vec2` | `vec2` | Returns a / b |
| `divVC(a,b)` | `a:vec2`, `b:number` | `vec2` | Returns a / b |
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
| `mod(x,y)` | `x:number`, `y:number` | `number` | Returns x modulo y (computed by x - y * floor(x/y)) |
| `remap(value,low1,high1,low2,high2)` | `value:number`, `low1:number`, `high1:number`, `low2:number`, `high2:number` | `number` | Returns value remapped from the first range to the second |
| `saturate(x)` | `x:number` | `number` | Returns x clamped in the range 0 <= x <= 1 |
| | | |
| **2-Dim Misc. Math Functions** |
| `floorV(v)` | `v:vec2` | `vec2` | Returns a vec2 where each component has been floored |
| `clampV(v,mi,ma)` | `v:vec2`, `mi:vec2`, `ma:vec2` | `vec2` | Returns a vec2 where each component has been clamped between the same component of mi and ma |
| `modVC(v,a)` | `v:vec2`, `a:number` | `vec2` | Returns v where each component is it's modulo with a |
| `fractV(v)` | `v:vec2` | `vec2` | Returns a vec2 where each component is it's fractional part |
| `cosV(v)` | `v:vec2` | `vec2` | Returns a vec2 where each component is the cosine of the original component |
| `sinV(v)` | `v:vec2` | `vec2` | Returns a vec2 where each component is the sine of the original component |
| `smoothstepVC(edge0,edge1,v)` | `edge0:number`, `edge1:number`, `v:vec2` | `vec2` | Returns, for each component of the vector (c), smooth Hermite interpolation of c between 0 and 1 when edge0 < c < edge1 |
| | | |
| **Color Functions** |
| `col4(r,g,b,a)` | `r:number`, `g:number`, `b:number`, `a:number` | `col4` | Returns a Color4 with the specified components. |
| `addCol(a,b)` | `a:col4`, `b:col4` | `col4` | Returns a + b |
| `addColC(a,b)` | `a:col4`, `b:number` | `col4` | Returns a + b |
| `subCol(a,b)` | `a:col4`, `b:col4` | `col4` | Returns a - b |
| `subColC(a,b)` | `a:col4`, `b:number` | `col4` | Returns a - b |
| `mulCol(a,b)` | `a:col4`, `b:col4` | `col4` | Returns a * b (As Color4s are intended to store a value from 0-255, this will convert them to range 0-1 before multiplying then convert back to 0-255 after the operation) |
| `mulColC(a,b)` | `a:col4`, `b:number` | `col4` | Returns a * b |
| `divColC(a,b)` | `a:col4`, `b:number` | `col4` | Returns a / b (There is no function to divide two Color4s currently, only a function to divide a Color4 by a number) |
| `invertCol(c)` | `c:col4` | `col4` | Returns 255 - c |
| | | |
| **4-Dim Misc. Math Functions** |
| `floorCol(c)` | `c:col4` | `col4` | Returns a col4 where each component has been floored |
| `clampColC(c,mi,ma)` | `c:col4`, `mi:number`, `ma:number` | `col4` | Returns a col4 where each component has been clamped between mi and ma |
| `modColC(c,a)` | `c:col4`, `a:number` | `col4` | Returns c where each component is it's modulo with a |
| | | |

> More functions will be added in the future.

## Examples
### Example 1 (Simple)
```lua
name "Constant Color"
color 0x8AE9C1FF
category "Core/Constants"

param color inCol "Color"
output colortex outTex "Out"

display outTex
```
> Constants/ConstantColor/.metadata
```lua
for x=1, sizeX do
    for y=1, sizeY do
        outTex[x][y] = inCol
    end
end
```
> Constants/ConstantColor/program.lua

#### Metadata
This metadata sets the Node's name to be 'Simple Colour', and gives it the colour 0x8AE9C1FF, which is RGBA(138, 233, 193, 255).   
It then defines two variables, one is `inCol`, which is a color parameter (meaning it can be edited in the inspector when the Node is selected), and the other is `outTex`, which is a colortex output (meaning it will be an output pin on the node).
#### Execution
This lua loops through every pixel in the texture, setting the pixel in `outTex` to be the parameter variable `inColor`, which is of type `color`. This will result in a texture with a solid color, that can be changed in the inspector.

### Example 2 (Complex)
```lua
name "Gradient"
color 0x9934EBFF
category "Core/Patterns"

param vec2 dir "Direction" 1.0,0.0
param bool bClamp "Clamp?" false
param int clampMin "Min" 0
param int clampMax "Max" 255
output greytex outTex "Out"

show clampMin (bClamp == true)
show clampMax (bClamp == true)

display outTex
```
> Patterns/Gradient/.metadata
```lua
local norm = normalizeV(dir)
local bb = dotV(norm,norm)
local max = lengthV(vec2(sizeX*norm.x,sizeY*norm.y))

for x=1, sizeX do
    for y=1, sizeY do
        local vec = vec2(x,y)
        local proj = mulVC(norm, dotV(vec, norm) / bb)
        local d = lengthV(vec2(proj.x,proj.y))
        local col = math.floor(clamp(d/max,0,1)*255)

        if bClamp then
            col = clamp(col, clampMin, clampMax)
        end

        outTex[x][y] = col
    end
end
```
> Patterns/Gradient/program.lua

#### Metadata
This metadata sets the Node's name to be 'Gradient', and gives it the colour 0x9934EBFF, which is RGBA(153, 52, 235, 255).   
It then defines a couple variables,   
`dir` - A `vec2` with the display name of `Direction` and a default value of `{x=1.0,y=0.0}`. This is a `param` so will be editable in the inspector.   
`bClamp` - A `bool` with the display name of `Clamp?` and a default value of `false`. This is also a `param`.
`clampMin` - A param `int` with the display name of `Min` and a default value of `0`.   
`clampMax` - A param `int` with the display name of `Max` and a default value of `255`.   
`outTex` - A `greytex` with the display name of `Out`. This is an `output`, so will be an output pin on the node which can be connected to other nodes.   

The metadata also defines `show` conditions for the two clamp variables, where they will only be shown in the editor if `bClamp` is true.
#### Execution
This lua first uses the Core Functions to calculate the normalized form of the `dir` variable, and calculates the dot product of itself. it also calculates the maximum length that the distance a pixel could be, from the top left. It then loops through each pixel of the texture, and, using the Core Functions to manipulate the vectors, calculates the end greyscale value (`col`) to be from 0 to 255. If needed, it will clamp this value, then sets the pixel in the `out` texture to be this greyscale value.


## If you need more clarification on how to use TGNL, please read the source code of the standard node library.

# Texture Graph Nodes

These are the core nodes for the Texture Graph app. They are written in the Texture Graph Node Language, and are loaded into the Library window, as well as compiled at runtime.

# TGNode Language v0.8 Documentation
> This language is not smart and requires specific spacing to work. Each token must be seperated by a space, and each keyword must be seperated by a newline.

A VSCode syntax extension can be found [here](https://github.com/oxi-dev0/TGNL)

## Table of Contents
- [Types](#types)
- [Overview](#overview)
  - [Metadata Definition](#metadata-definition)
  - [Variable Definition](#variable-definition)
  - [Execution Definition](#execution-definition)
- [Example](#example)

## Types
| Type | Definition | Example | Can be parameter? | Can be pin? |
| --- | --- | --- | --- | --- |
|`string` | A single line `"` surrounded string | `"Hello World"` | &check; | &cross; |
|`color` | A `0x` prefixed 8 char hex number that defines an RGBA color | `0xFF0000FF` | &check; | &check; |
|`int` | A single line integer | `324` | &check; | &check; |
|`float` | A single line float that includes a decimal point | `10.0` | &check; | &check; |
|`bool` | Either `true` or `false` | | &check; | &check; |
|`tex` | A 2D Array of `color` | | &cross; | &check; |
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
| | |
| `varname [luavar] [string]` | Defines the display name of a lua var. This is required for each lua var |
| `default [luavar] [data]` | Defines the default value of a parameter lua var. This is not required, but is only valid for parameter vars. `[data]`'s type depends on the variable's type |
| | |
| `display [luavar]` | Defines the lua var that should be used as the node preview. This is only valid for lua vars with type `tex`, `int`, `float`, `color`, or `bool` |
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
| `string` | `string` | |
| `float` | `float` | |
| `int` | `int` | |
| `bool` | `bool` | |
| `color` | `table` | Colors are formatted like this: `{r=255, g=0, b=0, a=255}`. These vars also get [metamethod overrides](http://lua-users.org/wiki/MetamethodsTutorial) for math operations. |
| `tex` | `2D Table of Color(table)` | An example of getting the red channel of the pixel color at (25,25) would be `Texture[25][25].r`. | 

### Other Vars
Again, before the execution lua is ran, these variables get defined to allow the lua code to interface with the node's data.  
These variables are extra and do not need to be defined in the `.tgnode` file.
| Variable | Type | Description |
| --- | --- | --- |
| `sizeX` | `int` | The graph texture width in pixels |
| `sizeY` | `int` | The graph texture height in pixels |
| `time` | `float` | The graph time in seconds |

## Example
> Ignore the syntax highlighting for the TGNL code.
```lua
metadata
    name "Solid Colour"
    color 0x8AE9C1FF

    varname Colour "Colour"
    varname Out "Out"

    default Colour 0xFF0000FF

    display Out
metadata

param color Colour
output tex Out

exec
{
    for x=1, sizeX do
        for y=1, sizeY do
            Out[x,y] = Colour
        end
    end
}
```
> SimpleSC.tgnode

#### Example metadata
This metadata sets the Node's name to be 'Simple Colour', and gives it the colour 0x8AE9C1FF, which is RGBA(138, 233, 193, 255). It then defines two display names for the variables `Colour` and `Out`. It sets `Colour`'s default value to 0xFF0000FF, which is RGBA(255, 0, 0, 255), and then specifies the variable `Out` to be the display variable for the Node. (It will show up on the Node in the graph).

#### Example variables
It then defines a variable called `Colour` with the type `color` and a variable called `Out` with the type `tex`.

#### Example execution
This lua loops through every pixel in the texture, setting the pixel in the out pin variable `Out`, which is of type `tex`, to be the parameter variable `Colour`, which is of type `color`. This will result in a texture with a solid color, that can be changed in the inspector.

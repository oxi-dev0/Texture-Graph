local outCopy = Out
local sizeXCopy = sizeX
local sizeYCopy = sizeY
if Invert then
Color.r = 255-Color.r
Color.g = 255-Color.g
Color.b = 255-Color.b
if InvertAlpha then
Color.a = 255-Color.a
end
end
local colorCopy = Color
for x=1, sizeXCopy do
for y=1, sizeYCopy do
outCopy[x][y] = colorCopy
end
end
Out = outCopy

if Invert then
Color.r = 255-Color.r
Color.g = 255-Color.g
Color.b = 255-Color.b
if InvertAlpha then
Color.a = 255-Color.a
end
end
for x=1, sizeX do
for y=1, sizeY do
Out[x][y] = Color
end
end

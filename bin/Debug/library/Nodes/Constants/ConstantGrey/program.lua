local val = intVal
if useFloat then
    val = math.floor(255*floatVal)
end

for x=1, sizeX do
    for y=1, sizeY do
        outTex[x][y] = val
    end
end
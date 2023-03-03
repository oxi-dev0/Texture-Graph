for x=1, sizeX do
    for y=1, sizeY do
        outTex[x][y] = math.floor(remap(inTex[x][y], inMin, inMax, outMin, outMax))
    end
end
for x=1, sizeX do
    for y=1, sizeY do
        local val = 0
        if inTex[x][y] >= threshold then
            val = 255
        end
        outTex[x][y] = val
    end
end
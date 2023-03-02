for x=1, sizeX do
    for y=1, sizeY do
        local o = inTex[x][y]

        contast = contrast * 0.5
        local low = saturate(position - contrast)
        local high = saturate(position + contrast);
        position = position * 2.0 - 1.0

        outTex[x][y] = math.floor(smoothstep(low, high, (o/255) + position) * 255)
    end
end
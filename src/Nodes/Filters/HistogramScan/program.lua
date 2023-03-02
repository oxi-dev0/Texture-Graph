contrast = contrast * 0.5
local low = saturate(position - contrast)
local high = saturate(position + contrast);
position = position * 2 - 1

for x=1, sizeX do
    for y=1, sizeY do
        local col = math.floor(smoothstep(low, high, (inTex[x][y]/255) + position) * 255)
        if invert then
            col = 255-col
        end
        outTex[x][y] = col
    end
end
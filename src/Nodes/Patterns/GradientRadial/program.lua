local middleX = sizeX/2
local middleY = sizeY/2
local max = middleX

for x=1, sizeX do
    for y=1, sizeY do
        local dist = lengthV(subV(vec2(middleX,middleY), vec2(x,y)))
        local col = math.floor((1-smoothstep(0,1,dist/max))*255)
        outTex[x][y] = col
    end
end
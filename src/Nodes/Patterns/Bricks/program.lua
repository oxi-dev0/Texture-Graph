local gapPx = gap
local brickX = (sizeX - (gapPx*(bricksPerRow-1)))/bricksPerRow
local brickY = brickX / ratio

local brickXC = brickX
local brickYC = brickY

for x=1, sizeX do
    for y=1, sizeY do
        local yOffset = (brickXC*0.5)*(math.floor(y/(brickYC+gapPx))%2)
        local posX = ((x+yOffset)%(brickXC+gapPx))
        local posY = ((y)%(brickYC+gapPx))
        local valid = (posX <= brickXC) and (posY <= brickYC)
        local val = 255
        if not valid then
            val = 0
        end
        outTex[x][y] = val
    end
end
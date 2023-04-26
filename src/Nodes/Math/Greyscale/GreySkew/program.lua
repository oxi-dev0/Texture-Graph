local mid = {x=math.floor(sizeX/2), y=math.floor(sizeY/2)}

local centerPos = mid.x
if anchor == "Top Left" then
    centerPos = 0
elseif anchor == "Bottom Right" then
    centerPos = sizeX
end

function sample(x, y)
    if rep then
        return inpTex[(x-1)%sizeX + 1][(y-1)%sizeY + 1]
    end

    if x < 1 then
        return 0
    end
    if y < 1 then
        return 0
    end
    if x > sizeX then
        return 0
    end
    if y > sizeY then
        return 0
    end

    return inpTex[x][y]
end

for x=1, sizeX do
    for y=1, sizeY do
        if dir == "Horizontal" then
            local coefficient = (y-centerPos)/mid.y
            outTex[x][y] = sample(math.floor(x-(sizeX*coefficient*amount)), y)
        elseif dir == "Vertical" then
            local coefficient = (x-centerPos)/mid.x
            outTex[x][y] = sample(x, math.floor(y-(sizeY*coefficient*amount)))
        end
    end
end
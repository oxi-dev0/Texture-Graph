function sample(x, y)
    if rep then
        return inp[(x-1)%sizeX + 1][(y-1)%sizeY + 1]
    end

    if x < 1 then
        return {r=0,g=0,b=0,a=0}
    end
    if y < 1 then
        return {r=0,g=0,b=0,a=0}
    end
    if x > sizeX then
        return {r=0,g=0,b=0,a=0}
    end
    if y > sizeY then
        return {r=0,g=0,b=0,a=0}
    end

    return inp[x][y]
end

function rotate(vec, angle)
    -- angle in rad
    -- newX = oldX * cos(angle) - oldY * sin(angle)
    -- newY = oldX * sin(angle) + oldY * cos(angle)
    local newX = vec.x * math.cos(angle) - vec.y * math.sin(angle)
    local newY = vec.x * math.sin(angle) + vec.y * math.cos(angle)
    return {x=newX, y=newY}
end

local mid = {x=math.floor(sizeX/2), y=math.floor(sizeY/2)}

for x=1, sizeX do
    for y=1, sizeY do
        local posTransPos = {x=x + math.floor(posX*sizeX), y=y + math.floor(posY*sizeY)}
        local vec = {x=posTransPos.x-mid.x,y=posTransPos.y-mid.y}
        vec = {x=vec.x*scaleX,y=vec.y*scaleY}
        local rotVec = rotate(vec, math.rad(angle))
        local newPos = {x=mid.x + rotVec.x,y=mid.y + rotVec.y}
        local sampled = sample(math.floor(newPos.x), math.floor(newPos.y))
        outTex[x][y] = sampled
    end
end
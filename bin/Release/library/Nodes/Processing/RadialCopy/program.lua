function sample(x, y)
    if rep then
        return inp[(x-1)%sizeX + 1][(y-1)%sizeY + 1]
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

function overlay(x,y,val)
    if overlayMode == "Add" then
        outTex[x][y] = clamp(outTex[x][y] + val, 0, 255)
    elseif overlayMode == "Greatest" then
        outTex[x][y] = math.max(outTex[x][y], val)
    elseif overlayMode == "Multiply" then
        outTex[x][y] = (outTex[x][y]/255)*(val/255)
    end
end

local pivot = {x=math.floor(sizeX*pivotVec.x), y=math.floor(sizeY*pivotVec.y)}
local angleBetween = (2*math.pi)/copies

for copy=1, copies do
    for x=1, sizeX do
        for y=1, sizeY do
            local vec = subV(vec2(x,y),pivot)
            local samplePos = rotate(vec, -angleBetween*copy)
            samplePos = addV(pivot,floorV(samplePos))
            overlay(x,y,sample(samplePos.x, samplePos.y))
        end
    end
end
local points = {}
for p=1, pointCount do
    points[p] = vec2(math.random(sizeX),math.random(sizeY))
end

local maxDist = lengthV(vec2(sizeX/2,sizeY/2))

function getClosest(x, y)
    local closestDist = 1000000000
    for p=1, pointCount do
        local dist = lengthV(subV(vec2(x,y),points[p]))
        if dist < closestDist then
            closestDist = dist
        end
    end
    return closestDist
end

for x=1, sizeX do
    for y=1, sizeY do
        local dist = getClosest(x,y)
        outTex[x][y] = 255-clamp(math.floor(remap(dist, 0, maxDist, 0, falloff)*255),0,255)
    end
end
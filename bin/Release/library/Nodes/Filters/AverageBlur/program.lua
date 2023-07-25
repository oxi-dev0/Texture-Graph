-- https://github.com/Jam3/glsl-fast-gaussian-blur

function sample(target,pos)
    return target[((math.floor(pos.x)-1)%sizeX)+1][((math.floor(pos.y)-1)%sizeY)+1]
end

function gatherKernal(pos)
    local totalCol = 0
    local numCols = 0
    for x=pos.x-radius,pos.x+radius,1 do
        for y=pos.y-radius,pos.y+radius,1 do
            numCols = numCols + 1
            totalCol = totalCol + sample(inTex,vec2(x,y))
        end
    end
    return math.floor(totalCol/numCols)
end

for x=1, sizeX do
    for y=1, sizeY do
        outTex[x][y] = gatherKernal(vec2(x,y))
    end
end
       
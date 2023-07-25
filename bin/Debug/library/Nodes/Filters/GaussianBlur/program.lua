-- https://github.com/Jam3/glsl-fast-gaussian-blur

function sample(target,pos)
    return target[((math.floor(pos.x)-1)%sizeX)+1][((math.floor(pos.y)-1)%sizeY)+1]
end

function lvl5(sampleTarget,pos,direction)
    local off1 = divV(mulV(vec2(1.3333333333333333,1.3333333333333333), direction), vec2(sizeX,sizeY))
    local col = col4(0,0,0,0)
    col = addCol(col, mulColC(sample(sampleTarget, pos), 0.29411764705882354))
    col = addCol(col, mulColC(sample(sampleTarget, addV(pos,off1)), 0.35294117647058826))
    col = addCol(col, mulColC(sample(sampleTarget, subV(pos,off1)), 0.35294117647058826))
    return floorCol(col)
end

function lvl9(sampleTarget,pos,direction)
    local off1 = divV(mulV(vec2(1.3846153846,1.3846153846), direction), vec2(sizeX,sizeY))
    local off2 = divV(mulV(vec2(3.2307692308,3.2307692308), direction), vec2(sizeX,sizeY))
    local col = col4(0,0,0,0)
    col = addCol(col, mulColC(sample(sampleTarget,pos), 0.2270270270))
    col = addCol(col, mulColC(sample(sampleTarget,addV(pos,off1)), 0.3162162162))
    col = addCol(col, mulColC(sample(sampleTarget,subV(pos,off1)), 0.3162162162))
    col = addCol(col, mulColC(sample(sampleTarget,addV(pos,off2)), 0.0702702703))
    col = addCol(col, mulColC(sample(sampleTarget,subV(pos,off2)), 0.0702702703))
    return floorCol(col)
end

function lvl13(sampleTarget,pos,direction)
    local off1 = divV(mulV(vec2(1.411764705882353,1.411764705882353), direction), vec2(sizeX,sizeY))
    local off2 = divV(mulV(vec2(3.2941176470588234,3.2941176470588234), direction), vec2(sizeX,sizeY))
    local off3 = divV(mulV(vec2(5.176470588235294,5.176470588235294), direction), vec2(sizeX,sizeY))
    local col = col4(0,0,0,0)
    col = addCol(col, mulColC(sample(sampleTarget,pos), 0.1964825501511404))
    col = addCol(col, mulColC(sample(sampleTarget,addV(pos,off1)), 0.2969069646728344))
    col = addCol(col, mulColC(sample(sampleTarget,subV(pos,off1)), 0.2969069646728344))
    col = addCol(col, mulColC(sample(sampleTarget,addV(pos,off2)), 0.09447039785044732))
    col = addCol(col, mulColC(sample(sampleTarget,subV(pos,off2)), 0.09447039785044732))
    col = addCol(col, mulColC(sample(sampleTarget,addV(pos,off3)), 0.010381362401148057))
    col = addCol(col, mulColC(sample(sampleTarget,subV(pos,off3)), 0.010381362401148057))
    return floorCol(col)
end

-- Put conditions outside of loop to save instructions
if dir == "Both" then
    if radius == "Small" then
        for x=1, sizeX do
            for y=1, sizeY do
                outTex[x][y] = lvl5(inTex,vec2(x,y),vec2(1,0))
            end
        end
        for x=1, sizeX do
            for y=1, sizeY do
                outTex[x][y] = lvl5(outTex,vec2(x,y),vec2(0,1))
            end
        end
    elseif radius == "Medium" then
        for x=1, sizeX do
            for y=1, sizeY do
                outTex[x][y] = lvl9(inTex,vec2(x,y),vec2(1,0))
            end
        end
        for x=1, sizeX do
            for y=1, sizeY do
                outTex[x][y] = lvl9(outTex,vec2(x,y),vec2(0,1))
            end
        end
    elseif radius == "Large" then
        for x=1, sizeX do
            for y=1, sizeY do
                outTex[x][y] = lvl13(inTex,vec2(x,y),vec2(1,0))
            end
        end
        for x=1, sizeX do
            for y=1, sizeY do
                outTex[x][y] = lvl13(outTex,vec2(x,y),vec2(0,1))
            end
        end
    end
elseif dir == "X" then
    if radius == "Small" then
        for x=1, sizeX do
            for y=1, sizeY do
                outTex[x][y] = lvl5(inTex,vec2(x,y),vec2(1,0))
            end
        end
    elseif radius == "Medium" then
        for x=1, sizeX do
            for y=1, sizeY do
                outTex[x][y] = lvl9(inTex,vec2(x,y),vec2(1,0))
            end
        end
    elseif radius == "Large" then
        for x=1, sizeX do
            for y=1, sizeY do
                outTex[x][y] = lvl13(inTex,vec2(x,y),vec2(1,0))
            end
        end
    end
elseif dir == "Y" then
    if radius == "Small" then
        for x=1, sizeX do
            for y=1, sizeY do
                outTex[x][y] = lvl5(inTex,vec2(x,y),vec2(0,1))
            end
        end
    elseif radius == "Medium" then
        for x=1, sizeX do
            for y=1, sizeY do
                outTex[x][y] = lvl9(inTex,vec2(x,y),vec2(0,1))
            end
        end
    elseif radius == "Large" then
        for x=1, sizeX do
            for y=1, sizeY do
                outTex[x][y] = lvl13(inTex,vec2(x,y),vec2(0,1))
            end
        end
    end
end
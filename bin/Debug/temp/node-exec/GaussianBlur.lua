-- [PREGENERATED CORE] --
-- These are a bunch of useful functions that can be used in TGNL exec definitions

-- VECTOR2
function vec2(x,y)
    return {x=x,y=y}
end

function addV(a,b)
    return {x=a.x+b.x,y=a.y+b.y}
end

function addVC(a,b)
    return {x=a.x+b,y=a.y+b}
end

function subV(a,b)
    return {x=a.x-b.x,y=a.y-b.y}
end

function subVC(a,b)
    return {x=a.x-b,y=a.y-b}
end

function mulV(a,b)
    return {x=a.x*b.x,y=a.y*b.y}
end

function mulVC(a,b)
    return {x=a.x*b,y=a.y*b}
end

function divV(a,b) -- technically wrong
    return {x=a.x/b.x,y=a.y/b.y}
end

function divVC(a,b)
    return {x=a.x/b,y=a.y/b}
end

function negateV(v)
    return {x=-v.x,y=-v.y}
end

function lengthV(v)
    return math.sqrt((v.x * v.x) + (v.y * v.y))
end

function normalizeV(v)
    local len = lengthV(v)
    return {x=v.x/len,y=v.y/len}
end

function dotV(a,b)
    return a.x * b.x + a.y * b.y
end

function crossV(a,b)
    return a.x * b.y - a.y * b.x;
end

-- FLOAT
function clamp(v,mi,ma)
    local mia = math.min(mi,ma)
    local maa = math.max(mi,ma)
    local v1 = v
    if v1 > maa then
        v1 = maa
    end
    if v1 < mia then
        v1 = mia
    end
    return v1
end

function step(edge, x)
    if x < edge then
        return 0
    end
    return 1
end

function mix(a,b,v)
    return a*(1-v)+b*v
end

function smoothstep(edge0,edge1,x)
    local t = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0)
    return t * t * (3 - 2 * t)
end

function fract(x)
    return x - math.floor(x)
end

function mod(x,y)
    return x - y * math.floor(x/y)
end

function remap(value, low1, high1, low2, high2)
	return low2 + (value - low1) * (high2 - low2) / (high1 - low1)
end

function saturate(x)
    return math.min(math.max(x, 0), 1)
end

-- VECTOR2 FLOAT UTIL
function floorV(v)
    return {x=math.floor(v.x),y=math.floor(v.y)}
end

function clampV(v, mi, ma)
    return {x=clamp(v.x, mi.x, ma.x), y=clamp(v.y, mi.y, ma.y)}
end

function modVC(v, a)
    return {x=mod(v.x,a),y=mod(v.y,a)}
end

function fractV(v)
    return {x=v.x-math.floor(v.x),y=v.y-math.floor(v.y)}
end

function cosV(v)
    return {x=math.cos(v.x),y=math.cos(v.y)}
end

function smoothstepVC(edge0,edge1,v)
    local tx = clamp((v.x - edge0) / (edge1 - edge0), 0.0, 1.0)
    local ty = clamp((v.y - edge0) / (edge1 - edge0), 0.0, 1.0)
    return vec2(tx * tx * (3 - 2 * tx), ty * ty * (3 - 2 * ty))
end

-- COLOR4
function col4(r,g,b,a)
    return {r=r,g=g,b=b,a=a}
end

function addCol(a,b)
    return {r=a.r+b.r,g=a.g+b.g,b=a.b+b.b,a=a.a+b.a}
end

function addColC(a,b)
    return {r=a.r+b,g=a.g+b,b=a.b+b,a=a.a+b}
end

function subCol(a,b)
    return {r=a.r-b.r,g=a.g-b.g,b=a.b-b.b,a=a.a-b.a}
end

function subColC(a,b)
    return {r=a.r-b,g=a.g-b,b=a.b-b,a=a.a-b}
end

function mulCol(a,b)
    return {r=((a.r/255)*(b.r/255))*255,g=((a.g/255)*(b.g/255))*255,b=((a.b/255)*(b.b/255))*255,a=((a.a/255)*(b.a/255))*255}
end

function mulColC(a,b)
    return {r=a.r*b,g=a.g*b,b=a.b*b,a=a.a*b}
end

function divColC(a,b)
    return {r=a.r/b,g=a.g/b,b=a.b/b,a=a.a/b}
end

function invertCol(c)
    return {r=255-c.r,g=255-c.g,b=255-c.b,a=255-c.a}
end

-- COLOR4 FLOAT UTIL
function floorCol(c)
    return {r=math.floor(c.r),g=math.floor(c.g),b=math.floor(c.b),a=math.floor(c.a)}
end

function clampCol(c, mi, ma)
    return {r=clamp(c.r,mi.ma),g=clamp(c.g,mi,ma),b=clamp(c.b,mi,ma),a=clamp(c.a,mi,ma)}
end

function modCol(c, a)
    return {r=mod(c.r,a),g=mod(c.g,a),b=mod(c.b,a),a=mod(c.a,a)}
end

-- [VARIABLE COPY (IMPROVES PERF)] --
local sizeX = g_sizeX
local sizeY = g_sizeY
local dir = g_dir
local inTex = g_inTex
local outTex = g_outTex
local radius = g_radius
-- [NODE EXEC] --
-- https://github.com/Jam3/glsl-fast-gaussian-blur

function sample(target,pos)
    print(pos.x)
    print(pos.y)
    return target[((pos.x-1)%sizeX)+1][((pos.y-1)%sizeY)+1]
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
-- [RESOLVE GLOBALS] --
g_outTex = outTex

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

-- [VARIABLE COPY (IMPROVES PERF)] --
local sizeX = g_sizeX
local sizeY = g_sizeY
local copies = g_copies
local inp = g_inp
local outTex = g_outTex
local overlayMode = g_overlayMode
local pivotVec = g_pivotVec
local rep = g_rep
-- [NODE EXEC] --
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
    else 
        if overlayMode == "Multiply" then
            outTex[x][y] = (outTex[x][y]/255)*(val/255)
        end
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
-- [RESOLVE GLOBALS] --
g_outTex = outTex

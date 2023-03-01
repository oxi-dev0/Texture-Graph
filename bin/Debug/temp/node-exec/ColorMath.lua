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
    local t = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    return t * t * (3 - 2 * t);
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

-- VECTOR2 FLOAT UTIL
function floorV(v)
    return {x=math.floor(v.x),y=math.floor(v.y)}
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
    local tx = clamp((v.x - edge0) / (edge1 - edge0), 0.0, 1.0);
    local ty = clamp((v.y - edge0) / (edge1 - edge0), 0.0, 1.0);
    return vec2(tx * tx * (3 - 2 * tx), ty * ty * (3 - 2 * ty));
end

-- [VARIABLE COPY (IMPROVES PERF)] --
local sizeX = g_sizeX
local sizeY = g_sizeY
local A = g_A
local B = g_B
local max = g_max
local min = g_min
local op = g_op
local outTex = g_outTex
local shouldClamp = g_shouldClamp
local useAlpha = g_useAlpha
-- [NODE EXEC] --
local clampMin = {r=0,g=0,b=0,a=0}
local clampMax = {r=255,g=255,b=255,a=255}
if shouldClamp then
    clampMin = min
    clampMax = max
end

if op == "Add" then
    for x=1, sizeX do
        for y=1, sizeY do
            local res = {}
            res.r = clamp(math.floor(((A[x][y].r/255) + (B[x][y].r/255))*255), clampMin.r, clampMax.r)
            res.g = clamp(math.floor(((A[x][y].g/255) + (B[x][y].g/255))*255), clampMin.g, clampMax.g)
            res.b = clamp(math.floor(((A[x][y].b/255) + (B[x][y].b/255))*255), clampMin.b, clampMax.b)
            if useAlpha then
                res.a = clamp(math.floor(((A[x][y].a/255) + (B[x][y].a/255))*255), clampMin.a, clampMax.a)
            else
                res.a = math.max(A[x][y].a, B[x][y].a)
            end
            outTex[x][y] = res
        end
    end
elseif op == "Subtract" then
    for x=1, sizeX do
        for y=1, sizeY do
            local res = {}
            res.r = clamp(math.floor(((A[x][y].r/255) - (B[x][y].r/255))*255), clampMin.r, clampMax.r)
            res.g = clamp(math.floor(((A[x][y].g/255) - (B[x][y].g/255))*255), clampMin.g, clampMax.g)
            res.b = clamp(math.floor(((A[x][y].b/255) - (B[x][y].b/255))*255), clampMin.b, clampMax.b)
            if useAlpha then
                res.a = clamp(math.floor(((A[x][y].a/255) - (B[x][y].a/255))*255), clampMin.a, clampMax.a)
            else
                res.a = math.max(A[x][y].a, B[x][y].a)
            end
            outTex[x][y] = res
        end
    end
elseif op == "Multiply" then
    for x=1, sizeX do
        for y=1, sizeY do
            local res = {}
            res.r = clamp(math.floor(((A[x][y].r/255) * (B[x][y].r/255))*255), clampMin.r, clampMax.r)
            res.g = clamp(math.floor(((A[x][y].g/255) * (B[x][y].g/255))*255), clampMin.g, clampMax.g)
            res.b = clamp(math.floor(((A[x][y].b/255) * (B[x][y].b/255))*255), clampMin.b, clampMax.b)
            if useAlpha then
                res.a = clamp(math.floor(((A[x][y].a/255) * (B[x][y].a/255))*255), clampMin.a, clampMax.a)
            else
                res.a = math.max(A[x][y].a, B[x][y].a)
            end
            outTex[x][y] = res
        end
    end
elseif op == "Divide" then
    for x=1, sizeX do
        for y=1, sizeY do
            local res = {}
            res.r = clamp(math.floor(((A[x][y].r/255) / (B[x][y].r/255))*255), clampMin.r, clampMax.r)
            res.g = clamp(math.floor(((A[x][y].g/255) / (B[x][y].g/255))*255), clampMin.g, clampMax.g)
            res.b = clamp(math.floor(((A[x][y].b/255) / (B[x][y].b/255))*255), clampMin.b, clampMax.b)
            if useAlpha then
                res.a = clamp(math.floor(((A[x][y].a/255) / (B[x][y].a/255))*255), clampMin.a, clampMax.a)
            else
                res.a = math.max(A[x][y].a, B[x][y].a)
            end
            outTex[x][y] = res
        end
    end
end
-- [RESOLVE GLOBALS] --
g_A = A
g_B = B
g_outTex = outTex

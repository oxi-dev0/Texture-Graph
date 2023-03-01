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
local inCol1 = g_inCol1
local inCol10 = g_inCol10
local inCol11 = g_inCol11
local inCol12 = g_inCol12
local inCol2 = g_inCol2
local inCol3 = g_inCol3
local inCol4 = g_inCol4
local inCol5 = g_inCol5
local inCol6 = g_inCol6
local inCol7 = g_inCol7
local inCol8 = g_inCol8
local inCol9 = g_inCol9
local invcodslvdfg = g_invcodslvdfg
local outTex1 = g_outTex1
local outTex10 = g_outTex10
local outTex11 = g_outTex11
local outTex12 = g_outTex12
local outTex2 = g_outTex2
local outTex3 = g_outTex3
local outTex4 = g_outTex4
local outTex5 = g_outTex5
local outTex6 = g_outTex6
local outTex7 = g_outTex7
local outTex8 = g_outTex8
local outTex9 = g_outTex9
local outghfdsiu4 = g_outghfdsiu4
-- [NODE EXEC] --
-- [RESOLVE GLOBALS] --
g_inCol1 = inCol1
g_inCol10 = inCol10
g_inCol11 = inCol11
g_inCol12 = inCol12
g_inCol2 = inCol2
g_inCol3 = inCol3
g_inCol4 = inCol4
g_inCol5 = inCol5
g_inCol6 = inCol6
g_inCol7 = inCol7
g_inCol8 = inCol8
g_inCol9 = inCol9
g_invcodslvdfg = invcodslvdfg
g_outTex1 = outTex1
g_outTex10 = outTex10
g_outTex11 = outTex11
g_outTex12 = outTex12
g_outTex2 = outTex2
g_outTex3 = outTex3
g_outTex4 = outTex4
g_outTex5 = outTex5
g_outTex6 = outTex6
g_outTex7 = outTex7
g_outTex8 = outTex8
g_outTex9 = outTex9
g_outghfdsiu4 = outghfdsiu4

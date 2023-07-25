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
local inTex = g_inTex
local outTex = g_outTex
-- [NODE EXEC] --
-- https://gist.github.com/Hebali/6ebfc66106459aacee6a9fac029d0115 (PROPER SOBEL, UNUSED)

function makeKernel(x,y)
    local kernel = {}

    local leftX = math.max(x-1,1)
    local rightX = math.min(x+1,sizeX)
    local upY = math.max(y-1,1)
    local downY = math.min(y+1,sizeY)

    kernel["tl"] = inTex[leftX][upY]
    kernel["t"] = inTex[x][upY]
    kernel["tr"] = inTex[rightX][upY]
    kernel["r"] = inTex[rightX][y]
    kernel["br"] = inTex[rightX][downY]
    kernel["b"] = inTex[x][downY]
    kernel["bl"] = inTex[leftX][downY]
    kernel["l"] = inTex[leftX][y]

    return kernel
end

function normal2color(x,y,z)
    local dist = 0.0
    local n = {r=0.0,g=0.0,b=0.0}
    dist = math.sqrt((x*x) + (y*y) + (z*z))
    n.r = x/dist
    n.g = y/dist
    n.b = z/dist

    local c = {r=0,g=0,b=0,a=255}
    c.r = math.floor((n.r + 1) * (255/2))
    c.g = math.floor((n.g + 1) * (255/2))
    c.b = math.floor((n.b + 1) * (255/2))
    return c
end

function avg(col)
    return 0.3*col.r + 0.59*col.g + 0.11*col.b
end

for x=1, sizeX do
    for y=1, sizeY do
        local n = makeKernel(x,y)

        local tl = avg(n["tl"])
        local t = avg(n["t"]);
        local tr = avg(n["tr"]);
        local r = avg(n["r"]);
        local br = avg(n["br"]);
        local b = avg(n["b"]);
        local bl = avg(n["bl"]);
        local l = avg(n["l"]);

        local dX = (tr + 2.0 * r + br) - (tl + 2.0 * l + bl);
        local dY = (bl + 2.0 * b + br) - (tl + 2.0 * t + tr);
        local dZ = 255;

        local color = normal2color(dX,-dY,dZ) -- flipped

        outTex[x][y] = color
    end
end
-- [RESOLVE GLOBALS] --
g_outTex = outTex

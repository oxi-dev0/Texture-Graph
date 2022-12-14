-- PREGENERATED CORE --
-- These are a bunch of useful functions that can be used in TGNL exec definitions

-- VECTOR
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
    return {x=a.x+b.x,y=a.y+b.y}
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

-- Multi-dim float util
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
-- PREGENERATED CORE --

-- NODE EXEC --
-- 2D Random
function rand(c) -- c:vec2
return fract(math.sin(dotV(c,vec2(12.9898,78.233))) * 43758.5453)
end
function noise(p, freq) -- p:vec2, freq:number
local unit = sizeX/freq
local ij = floorV(divVC(p,unit))
local xy = divVC(modVC(p,unit),unit)
--xy = 3.*xy*xy-2.*xy*xy*xy
xy = mulVC(subVC(negateV(cosV(mulVC(xy, math.pi))), 1), 0.5)
local a = rand(addV(ij,vec2(0,0)))
local b = rand(addV(ij,vec2(1.,0.)))
local c = rand(addV(ij,vec2(0.,1.)))
local d = rand(addV(ij,vec2(1.,1.)))
local x1 = mix(a, b, xy.x)
local x2 = mix(c, d, xy.x)
return mix(x1, x2, xy.y)
end
function pNoise(p, res) -- p:vec2, res:number
local persistance = 0.5
local n = 0
local normK = 0
local f = 4
local amp = 1
local iCount = 0
local i = 0
while i < 50 do
n = n + (amp*noise(p, f))
f = f * 2
normK = normK + amp
amp = amp * persistance
if (iCount == res) then break end
iCount = iCount + 1
i = i + 1
end
local nf = n/normK
return nf*nf*nf*nf
end
for x=1, sizeX do
for y=1, sizeY do
Out[x][y] = math.floor(pNoise(vec2(x/sizeX, y/sizeY), res)*255)
end
end
-- NODE EXEC --
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
function sample(x, y)
if rep then
return inp[(x-1)%sizeX + 1][(y-1)%sizeY + 1]
end
if x < 1 then
return {r=0,g=0,b=0,a=0}
end
if y < 1 then
return {r=0,g=0,b=0,a=0}
end
if x > sizeX then
return {r=0,g=0,b=0,a=0}
end
if y > sizeY then
return {r=0,g=0,b=0,a=0}
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
local mid = {x=math.floor(sizeX/2), y=math.floor(sizeY/2)}
for x=1, sizeX do
for y=1, sizeY do
local posTransPos = {x=x + math.floor(posX*sizeX), y=y + math.floor(posY*sizeY)}
local vec = {x=posTransPos.x-mid.x,y=posTransPos.y-mid.y}
vec = {x=vec.x*scaleX,y=vec.y*scaleY}
local rotVec = rotate(vec, math.rad(angle))
local newPos = {x=mid.x + rotVec.x,y=mid.y + rotVec.y}
local sampled = sample(math.floor(newPos.x), math.floor(newPos.y))
out[x][y] = sampled
end
end
-- NODE EXEC --
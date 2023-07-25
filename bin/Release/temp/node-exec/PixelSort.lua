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
local algo = g_algo
local comp = g_comp
local dir = g_dir
local inTex = g_inTex
local iters = g_iters
local outTex = g_outTex
-- [NODE EXEC] --
function compareSum(a,b)
    local sA = a.r + a.g + a.b + a.a
    local sB = b.r + b.g + b.b + b.a
    if sA > sB then return 1 end
    if sB > sA then return -1 end
    if sA == sB then return 0 end
end

function compareAvg(a,b)
    local aA = (a.r+a.g+a.b+a.a)/4
    local aB = (b.r+b.g+b.b+b.a)/4
    if aA > aB then return 1 end
    if aB > aA then return -1 end
    if aA == aB then return 0 end
end

function compareGrey(a,b)
    local gA = (a.r * 0.3 + a.g * 0.59 + a.b * 0.11) * (a.a/255)
    local gB = (b.r * 0.3 + b.g * 0.59 + b.b * 0.11) * (b.a/255)
    if gA > gB then return 1 end
    if gB > gA then return -1 end
    if gA == gB then return 0 end
end

function compare(a,b)
    if comp == "Sum" then return compareSum(a,b) end
    if comp == "Average" then return compareAvg(a,b) end
    if comp == "Greyscale" then return compareGrey(a,b) end
    return 0
end

function bubbleSort(list, invert)
    local target = -1
    if invert == true then
        target = 1
    end

    local pass = 1
    while pass <= iters do
        local i = 1
        while i <= #list do
            local j = i+1
            while j <= #list do
                if compare(list[i], list[j]) == target then
                    local temp = list[j]
                    list[j] = list[i]
                    list[i] = temp
                end
                j=j+1
            end
            i=i+1
        end
        pass=pass+1
    end
end

function gnomeSort(list, invert)
    local target = -1
    if invert == true then
        target = 1
    end

    local pass = 1
    while pass <= iters do
        local position = 2
        while position <= #list do
            if compare(list[position-1], list[position]) == target then
                local temp = list[position]
                list[position] = list[position-1]
                list[position-1] = temp
                position = position - 1
                if position < 2 then position = 2 end
            else
                position = position + 1
            end
        end
        pass=pass+1
    end
end

function insertionSort(list, invert, debug)
    local target = -1
    if invert == true then
        target = 1
    end

    local pass = 1
    while pass <= iters do
        local i = 2
        while i <= #list do
            local j = i
            while j > 1 and compare(list[j-1], list[j]) == target do
                if debug == true then
                    print("Swap: " .. tostring(j-1) .. "<->" .. tostring(j))
                end
                local temp = list[j]
                list[j] = list[j-1]
                list[j-1] = temp
                j = j - 1
            end
            i=i+1
        end
        pass=pass+1
    end
end

function sort(list, invert, debug)
    if algo == "Bubble Sort" then
        bubbleSort(list, invert)
    elseif algo == "Gnome Sort" then
        gnomeSort(list, invert)
    elseif algo == "Insertion Sort" then
        insertionSort(list, invert, debug)
    end
end

if dir == "Top to Bottom" or dir == "Bottom to Top" then
    for x=1, sizeX do
        sort(inTex[x], dir=="Bottom to Top")
    end
    outTex = inTex
end
if dir == "Left to Right" or dir == "Right to Left" then
    for y=1, sizeY do
        list = {}
        for x=1, sizeX do
            table.insert(list, inTex[x][y])
        end
        sort(list, dir=="Right to Left", (y==255))
        for x=1, sizeX do
            outTex[x][y] = list[x]
        end
    end
end
-- [RESOLVE GLOBALS] --
g_outTex = outTex

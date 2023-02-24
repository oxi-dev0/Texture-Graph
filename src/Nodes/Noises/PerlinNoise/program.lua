function noise(x,y) 
    return 2 * fract(math.sin(dotV(vec2(x,y),vec2(12.9898,78.233))) * 43758.5453) - 1
end

function smoothNoise(x,y)
    return noise(x,y)/4.0+(noise(x+1,y)+noise(x-1,y)+noise(x,y+1)+noise(x,y-1))/8.0+(noise(x+1,y+1)+noise(x+1,y-1)+noise(x-1,y+1)+noise(x-1,y-1))/16.0
end

function COSInterpolation(x,y,n)
    local r = n*3.1415926
    local f = (1.0-math.cos(r))*0.5
    return x*(1.0-f)+y*f
end

function InterpolationNoise(x,y)
    local ix = math.floor(x)
    local iy = math.floor(y)
    local fracx = x-ix
    local fracy = y-iy
    
    local v1 = smoothNoise(ix,iy)
    local v2 = smoothNoise(ix+1,iy)
    local v3 = smoothNoise(ix,iy+1)
    local v4 = smoothNoise(ix+1,iy+1)
    
    local i1 = COSInterpolation(v1,v2,fracx)
    local i2 = COSInterpolation(v3,v4,fracx)
    
    return COSInterpolation(i1,i2,fracy)
end

function PerlinNoise2D(x,y)
    local sum = 0
    local frequency = 0
    local amplitude = 0
    local i = firstOctave
    while i < octaves do
        frequency = math.pow(2,i)
        amplitude = math.pow(persistence,i)
        sum = sum + InterpolationNoise(x*frequency,y*frequency)*amplitude
        i = i + 1
    end
    
    return sum;
end

for x=1, sizeX do
    for y=1, sizeY do
        local uv = divV(vec2(x,y), vec2(sizeX,sizeY))
        local noise = PerlinNoise2D(uv.x,uv.y) --0.3+0.7*PerlinNoise2D(uv.x,uv.y)
        outTex[x][y] = math.floor(((mult * noise) + offset)*255)
    end
end
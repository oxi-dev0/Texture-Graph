-- https://gist.github.com/Hebali/6ebfc66106459aacee6a9fac029d0115 (PROPER SOBEL, UNUSED)

function makeKernel(x,y)
    local kernel = {}

    local leftX = math.max(x-1,1)
    local rightX = math.min(x+1,sizeX)
    local upY = math.max(y-1,1)
    local downY = math.min(y+1,sizeY)

    kernel["tl"] = in[leftX][upY]
    kernel["t"] = in[x][upY]
    kernel["tr"] = in[rightX][upY]
    kernel["r"] = in[rightX][y]
    kernel["br"] = in[rightX][downY]
    kernel["b"] = in[x][downY]
    kernel["bl"] = in[leftX][downY]
    kernel["l"] = in[leftX][y]

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

        out[x][y] = color
    end
end
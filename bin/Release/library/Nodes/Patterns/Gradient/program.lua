local norm = normalizeV(dir)
local bb = dotV(norm,norm)
local max = lengthV(vec2(sizeX*norm.x,sizeY*norm.y))

for x=1, sizeX do
    for y=1, sizeY do
        local vec = vec2(x,y)
        local proj = mulVC(norm, dotV(vec, norm) / bb)
        local d = lengthV(vec2(proj.x,proj.y))
        local col = math.floor(smoothstep(0,1,d/max)*255)

        if bClamp then
            col = clamp(col, clampMin, clampMax)
        end

        outTex[x][y] = col
    end
end
-- https://www.shadertoy.com/view/XsfSDs#
for x=1, sizeX do
    for y=1, sizeY do
        local uv = divV(vec2(x,y), vec2(sizeX, sizeY))
        uv = subV(uv, blurPoint)

        local precompute = blurWidth * (1.0 / (sampleCount-1))

        local color = {r=0, g=0, b=0, a=0}
        for i=1, sampleCount do
            local scale = 1 + (i*precompute)
            local newUv = floorV(mulV(addV(mulVC(uv, scale), blurPoint), vec2(sizeX, sizeY)))

            if not (newUv.x > 0 and newUv.x <= sizeX and newUv.y > 0 and newUv.y <= sizeY) then
                newUv = clampV(newUv, vec2(1, sizeX), vec2(1, sizeY))
            end
            
            color.r = color.r + inTex[newUv.x][newUv.y].r
            color.g = color.g + inTex[newUv.x][newUv.y].g
            color.b = color.b + inTex[newUv.x][newUv.y].b
            color.a = color.a + inTex[newUv.x][newUv.y].a
        end

        color.r = math.floor(color.r / sampleCount)
        color.g = math.floor(color.g / sampleCount)
        color.b = math.floor(color.b / sampleCount)
        color.a = math.floor(color.a / sampleCount)
        outTex[x][y] = color
    end
end
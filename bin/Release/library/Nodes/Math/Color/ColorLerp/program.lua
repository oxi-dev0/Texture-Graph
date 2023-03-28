function Lerp(a, b, t)
    return a + (b - a) * t
end

for x=1, sizeX do
    for y=1, sizeY do
        local alpha = T[x][y] / 255
        if invert then
            alpha = 1-alpha
        end
        local rn = math.floor(mix(A[x][y].r, B[x][y].r, alpha))
        local gn = math.floor(mix(A[x][y].g, B[x][y].g, alpha))
        local bn = math.floor(mix(A[x][y].b, B[x][y].b, alpha))
        local an = math.floor(mix(A[x][y].a, B[x][y].a, alpha))
        outTex[x][y] = {r=rn,g=gn,b=bn,a=an}
    end
end
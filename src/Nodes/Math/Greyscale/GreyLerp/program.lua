for x=1, sizeX do
    for y=1, sizeY do
        local alpha = T[x][y] / 255
        if invert then
            alpha = 1-alpha
        end
        outTex[x][y] = math.floor(mix(A[x][y], B[x][y], alpha))
    end
end
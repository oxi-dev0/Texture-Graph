patternSizeX = sizeFX
patternSizeY = sizeFY
if uniform then
    patternSizeX = sizeFU
    patternSizeY = sizeFU
end
patternSizeX = patternSizeX * sizeX
patternSizeY = patternSizeY * sizeY

for x=1, sizeX do
    local xSection = 0
    local xCursor = 0
    while xCursor < x do
        xSection = xSection + 1
        xCursor = xCursor + patternSizeX
    end
    local patternX = xSection % 2 == 0

    for y=1, sizeY do
        local ySection = 0
        local yCursor = 0
        while yCursor < y do
            ySection = ySection + 1
            yCursor = yCursor + patternSizeY
        end
        local patternY = ySection % 2 == 0

        local pattern = patternX ~= patternY
        local color = colA
        if pattern then
            color = colB
        end
        outTex[x][y] = color
    end
end
local clampMin = 0
local clampMax = 255
if shouldClamp then
    clampMin = min
    clampMax = max
end

if op == "Add" then
    for x=1, sizeX do
        for y=1, sizeY do
            outTex[x][y] = clamp(((A[x][y]/255) + (B[x][y]/255))*255, clampMin, clampMax)
        end
    end
elseif op == "Subtract" then
    for x=1, sizeX do
        for y=1, sizeY do
            outTex[x][y] = clamp(((A[x][y]/255) - (B[x][y]/255))*255, clampMin, clampMax)
        end
    end
elseif op == "Multiply" then
    for x=1, sizeX do
        for y=1, sizeY do
            outTex[x][y] = clamp(((A[x][y]/255) * (B[x][y]/255))*255, clampMin, clampMax)
        end
    end
elseif op == "Divide" then
    for x=1, sizeX do
        for y=1, sizeY do
            outTex[x][y] = clamp(((A[x][y]/255) / (B[x][y]/255))*255, clampMin, clampMax)
        end
    end
end
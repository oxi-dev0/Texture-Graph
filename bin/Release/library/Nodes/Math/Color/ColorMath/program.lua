local clampMin = {r=0,g=0,b=0,a=0}
local clampMax = {r=255,g=255,b=255,a=255}
if shouldClamp then
    clampMin = min
    clampMax = max
end

if op == "Add" then
    for x=1, sizeX do
        for y=1, sizeY do
            local res = {}
            res.r = clamp(math.floor(((A[x][y].r/255) + (B[x][y].r/255))*255), clampMin.r, clampMax.r)
            res.g = clamp(math.floor(((A[x][y].g/255) + (B[x][y].g/255))*255), clampMin.g, clampMax.g)
            res.b = clamp(math.floor(((A[x][y].b/255) + (B[x][y].b/255))*255), clampMin.b, clampMax.b)
            if useAlpha then
                res.a = clamp(math.floor(((A[x][y].a/255) + (B[x][y].a/255))*255), clampMin.a, clampMax.a)
            else
                res.a = math.max(A[x][y].a, B[x][y].a)
            end
            outTex[x][y] = res
        end
    end
elseif op == "Subtract" then
    for x=1, sizeX do
        for y=1, sizeY do
            local res = {}
            res.r = clamp(math.floor(((A[x][y].r/255) - (B[x][y].r/255))*255), clampMin.r, clampMax.r)
            res.g = clamp(math.floor(((A[x][y].g/255) - (B[x][y].g/255))*255), clampMin.g, clampMax.g)
            res.b = clamp(math.floor(((A[x][y].b/255) - (B[x][y].b/255))*255), clampMin.b, clampMax.b)
            if useAlpha then
                res.a = clamp(math.floor(((A[x][y].a/255) - (B[x][y].a/255))*255), clampMin.a, clampMax.a)
            else
                res.a = math.max(A[x][y].a, B[x][y].a)
            end
            outTex[x][y] = res
        end
    end
elseif op == "Multiply" then
    for x=1, sizeX do
        for y=1, sizeY do
            local res = {}
            res.r = clamp(math.floor(((A[x][y].r/255) * (B[x][y].r/255))*255), clampMin.r, clampMax.r)
            res.g = clamp(math.floor(((A[x][y].g/255) * (B[x][y].g/255))*255), clampMin.g, clampMax.g)
            res.b = clamp(math.floor(((A[x][y].b/255) * (B[x][y].b/255))*255), clampMin.b, clampMax.b)
            if useAlpha then
                res.a = clamp(math.floor(((A[x][y].a/255) * (B[x][y].a/255))*255), clampMin.a, clampMax.a)
            else
                res.a = math.max(A[x][y].a, B[x][y].a)
            end
            outTex[x][y] = res
        end
    end
elseif op == "Divide" then
    for x=1, sizeX do
        for y=1, sizeY do
            local res = {}
            res.r = clamp(math.floor(((A[x][y].r/255) / (B[x][y].r/255))*255), clampMin.r, clampMax.r)
            res.g = clamp(math.floor(((A[x][y].g/255) / (B[x][y].g/255))*255), clampMin.g, clampMax.g)
            res.b = clamp(math.floor(((A[x][y].b/255) / (B[x][y].b/255))*255), clampMin.b, clampMax.b)
            if useAlpha then
                res.a = clamp(math.floor(((A[x][y].a/255) / (B[x][y].a/255))*255), clampMin.a, clampMax.a)
            else
                res.a = math.max(A[x][y].a, B[x][y].a)
            end
            outTex[x][y] = res
        end
    end
end
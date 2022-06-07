function Lerp(a, b, t)
return a + (b - a) * t
end
for x=1, sizeX do
for y=1, sizeY do
local alpha = M[x][y] / 255
local rn = math.floor(Lerp(A[x][y].r, B[x][y].r, alpha))
local gn = math.floor(Lerp(A[x][y].g, B[x][y].g, alpha))
local bn = math.floor(Lerp(A[x][y].b, B[x][y].b, alpha))
local an = math.floor(Lerp(A[x][y].a, B[x][y].a, alpha))
Out[x][y] = {r=rn, g=gn, b=bn, a=an}
end
end

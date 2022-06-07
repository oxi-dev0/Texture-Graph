function Lerp(a, b, t)
return a + (b - a) * t
end
for x=1, sizeX do
for y=1, sizeY do
local alpha = M[x][y] / 255
Out[x][y] = math.floor(Lerp(A[x][y], B[x][y], alpha))
end
end

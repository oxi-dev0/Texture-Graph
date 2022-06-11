for x=1, sizeX do
for y=1, sizeY do
local c = {r=0,g=0,b=0,a=0}
c.r = math.floor((A[x][y].r/255) * (B[x][y].r))
c.g = math.floor((A[x][y].g/255) * (B[x][y].g))
c.b = math.floor((A[x][y].b/255) * (B[x][y].b))
c.a = math.floor((A[x][y].a/255) * (B[x][y].a))
Out[x][y] = c
end
end

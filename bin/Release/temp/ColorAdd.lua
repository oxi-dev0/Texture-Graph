for x=1, sizeX do
for y=1, sizeY do
local c = {r=0,g=0,b=0,a=0}
c.r = A[x][y].r + B[x][y].r
c.g = A[x][y].g + B[x][y].g
c.b = A[x][y].b + B[x][y].b
c.a = A[x][y].a + B[x][y].a
Out[x][y] = c
end
end
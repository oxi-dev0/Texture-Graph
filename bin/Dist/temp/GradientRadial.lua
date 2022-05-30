function distance ( x1, y1, x2, y2 )
local dx = x2 - x1
local dy = y2 - y1
return math.sqrt ( (dx * dx) + (dy * dy) )
end
function clamp(v,mi,ma)
local v1 = v
if v1 > ma then
v1 = ma
end
if v1 < mi then
v1 = mi
end
return v1
end
local middleX = sizeX/2
local middleY = sizeY/2
local max = distance(0,0,middleX,middleY)
for x=1, sizeX do
for y=1, sizeY do
local dist = distance(x,y,middleX,middleY)
local col = math.floor(clamp(1-(dist/(max/Falloff)),0,1)*255)
Out[x][y] = col
end
end

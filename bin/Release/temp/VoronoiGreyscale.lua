function distance ( x1, y1, x2, y2 )
local dx = x2 - x1
local dy = y2 - y1
return math.sqrt ( (dx * dx) + (dy * dy) )
end
local points = {}
for p=1, PointCount do
points[p] = {x=math.random(0,sizeX),y=math.random(0,sizeY)}
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
local maxDist = distance(sizeX/2,sizeY/2,0,0)
function getClosest (x, y)
local closestIndex = 0
local closestDist = 1000000000
for p=1, PointCount do
local dist = distance(x,y,points[p].x,points[p].y)
if dist < closestDist then
closestIndex = p
closestDist = dist
end
end
return points[closestIndex]
end
for x=1, sizeX do
for y=1, sizeY do
local closest = getClosest(x,y)
Out[x][y] = 255-clamp(math.floor((distance(x,y,closest.x,closest.y)/(maxDist/Falloff))*255),0,255)
end
end
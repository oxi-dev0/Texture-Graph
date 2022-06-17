function step(edge, x)
if x < edge then
return 0
end
return 1
end
function length(x)
return math.sqrt((x.x*x.x) + (x.y*x.y))
end
function polygonShape(position, radius, numSides)
position.x = position.x * 2 - 1
position.y = position.y * 2 - 1
local angle = math.atan(position.x, position.y)
local slice = math.pi * 2 / numSides
return 1-step(radius, math.cos(math.floor(0.5 + angle / slice) * slice - angle) * length(position))
end
for x=1, sizeX do
for y=1, sizeY do
local pos = {x=0,y=0}
pos.x = x / sizeX
pos.y = y / sizeY
Out[x][y] = math.floor(polygonShape(pos, radius, numSides)*255)
end
end
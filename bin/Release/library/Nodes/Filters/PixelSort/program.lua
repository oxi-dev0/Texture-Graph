function compare(a,b)
    local sA = a.r + a.g + a.b + a.a
    local sB = b.r + b.g + b.b + b.a
    if sA > sB then return 1 end
    if sB > sA then return -1 end
    if sA == sB then return 0 end
end

function bubbleSort(list, invert)
    local target = -1
    if invert == true then
        target = 1
    end

    local pass = 1
    while pass <= iters do
        local i = 1
        while i <= #list do
            local j = i+1
            while j <= #list do
                if compare(list[i], list[j]) == target then
                    local temp = list[j]
                    list[j] = list[i]
                    list[i] = temp
                end
                j=j+1
            end
            i=i+1
        end
        pass=pass+1
    end
end

function gnomeSort(list, invert)
    local target = -1
    if invert == true then
        target = 1
    end

    local pass = 1
    while pass <= iters do
        local position = 2
        while position <= #list do
            if compare(list[position-1], list[position]) == target then
                local temp = list[position]
                list[position] = list[position-1]
                list[position-1] = temp
                position = position - 1
                if position < 2 then position = 2 end
            else
                position = position + 1
            end
        end
        pass=pass+1
    end
end

function insertionSort(list, invert)
    local target = -1
    if invert == true then
        target = 1
    end

    local pass = 1
    while pass <= iters do
        local i = 2
        while i <= #list do
            local j = i
            while j > 1 and compare(list[j-1], list[j]) == target do
                local temp = list[j]
                list[j] = list[j-1]
                list[j-1] = temp
                j = j - 1
            end
            i=i+1
        end
        pass=pass+1
    end
end

function sort(list, invert)
    if algo == "Bubble Sort" then
        bubbleSort(list, invert)
    elseif algo == "Gnome Sort" then
        gnomeSort(list, invert)
    elseif algo == "Insertion Sort" then
        insertionSort(list, invert)
    end
end

if dir == "Top to Bottom" or dir == "Bottom to Top" then
    for x=1, sizeX do
        sort(inTex[x], dir=="Bottom to Top")
    end
    outTex = inTex
end
if dir == "Left to Right" or dir == "Right to Left" then
    for y=1, sizeY do
        list = {}
        for x=1, sizeX do
            table.insert(list, inTex[x][y])
        end
        sort(list, dir=="Right to Left")
        for x=1, sizeX do
            outTex[x][y] = list[x]
        end
    end
end
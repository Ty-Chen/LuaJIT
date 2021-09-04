local _M = {}

local N = 1e7

local function heavy()
    print("get into heavy")
    local sum = 0
    for i = 1, N do
        sum = sum + i
    end
    return sum
end

local function foo()
    local a = heavy()
    a = a + heavy()
    return a
end

local function bar()
    return (heavy())
end

function _M.main()
    io.write("get into main")
    foo()
    bar()
end

--c_hook.set_hook()
--_M.main()

return _M
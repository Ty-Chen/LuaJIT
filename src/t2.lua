local _M = {}

local N = 1e7

-- defines a factorial function
Counters = {}
Names = {}

function traceback ()
    for level = 1, math.huge do
        local info = debug.getinfo(level, "Sl")        
        if not info then 
            break 
        end
        if info.what == "C" then 
            -- is a C function?
            print(string.format("%d\tC function", level))
        else -- a Lua function
            print(string.format("%d\t[%s]:%d", level,
        info.short_src, info.currentline))
        end
    end
end

function hook ()
    print(debug.traceback())
    local f = debug.getinfo(2, "f").func
    local count = Counters[f]
    if count == nil then 
        -- first time 'f' is called?
        Counters[f] = 1
        Names[f] = debug.getinfo(2, "Sn")
    else 
        -- only increment the counter
        Counters[f] = count + 1
    end
end

function getname (func)
    local n = Names[func]
    if n.what == "C" then
        return n.name
    end
    local lc = string.format("[%s]:%d", n.short_src, n.linedefined)
    if n.what ~= "main" and n.namewhat ~= "" then
        return string.format("%s (%s)", lc, n.name)
    else
        return lc
    end
end

local function heavy()
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
    foo()
    bar()
end

debug.sethook(hook, "c")
_M.main()
debug.sethook()

for func, count in pairs(Counters) do
    print(getname(func), count)
end

return _M
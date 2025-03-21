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

function fact (n)
    if n == 0 then
        return 1
    else
        return n * fact(n - 1)
    end
end

print("enter a number:")
a = io.read("*n") -- reads a number

debug.sethook(hook, "c")
print(fact(a))
debug.sethook()

for func, count in pairs(Counters) do
    print(getname(func), count)
end
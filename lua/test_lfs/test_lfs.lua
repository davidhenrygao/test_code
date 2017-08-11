local lfs = require "lfs"
local string = require "string"
local io = require "io"

local file_path
local ONE_SPACE = "  "

local function gen_level_space(level)
    local spaces = ""
    for i=1,level do
        spaces = spaces .. ONE_SPACE
    end
    return spaces
end

local function path_base(path)
    if string.sub(path, -1) == "/" then
	path = path:sub(1, -2)
    end
    local rpath = path:reverse()
    local _, e = rpath:find("/", 1, true)
    if not e then
	e = rpath:len() + 1
    end
    return string.reverse(rpath:sub(1,e-1))
end

local travelfs 
travelfs = function(path, level)
    local attrs, errmsg = lfs.attributes(path)
    if attrs == nil then
        print(errmsg)
    end
    if attrs.mode == "directory" then
        print(gen_level_space(level) .. attrs.mode .. ": " .. path_base(path))
	for f in lfs.dir(path) do
	    if f ~= "." and f ~= ".." then
	        local new_path = path .. "/" .. f
		travelfs(new_path, level+1)
	    end
	end
    else
        print(gen_level_space(level) .. attrs.mode .. ": " .. path_base(path))
    end
end

file_path = io.stdin:read()
if file_path ~= nil then
    travelfs(file_path, 0)
end

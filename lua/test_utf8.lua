local forbidden_text = require "forbidden_text"

local function utf8_len(s)
    local count = 0
    for w in string.gmatch(s, "[%z\1-\127\194-\244][\128-\191]*") do
	count = count + 1
    end
    return count
end

for _,text in ipairs(forbidden_text) do
    print(text)
    print(utf8_len(text))
end

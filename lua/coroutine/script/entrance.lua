local c = require "serverop"

local tCoroutine = {}

local function getGenSessionFunc()
    local counter = 0
    local function genSession()
        counter = counter + 1
	return counter
    end
    return genSession
end

local genSession = getGenSessionFunc()

local function doRequest(req, dest)
  print("In lua doRequest, req:", req, ", dest", dest)
  local s = genSession()
  local co = coroutine.create(function (request, session, reqdest)
      print("In coroutine", session, ", req:", request)
      local resp, sess, respdest = c.call(request, session, reqdest)
      print("Coroutine", sess, "resume", ", resp:", resp)
      c.send(resp, respdest)
  end)
  coroutine.resume(co, req, s, dest)
  tCoroutine[s] = co
  print("lua doRequest end, req:", req)
end

local function doResponse(resp, session, dest)
  print("In lua doResponse, resp:", resp, ", session", session, ", dest", dest)
  local co = tCoroutine[session]
  if co == nil then
      print("No coroutine correspond session:", session)
  else
      coroutine.resume(co, resp, session, dest)
  end
  print("lua doResponse end, resp:", resp)
end

local function debugTrace(msg)
    print(debug.traceback(msg))
end

local handler = {
  doRequest = doRequest,
  doResponse = doResponse,
  debugTrace = debugTrace,
}

return handler

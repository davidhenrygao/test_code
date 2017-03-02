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

local function handleCoroutineResumeResult(co, ...)
  local result = select(1, ...)
  if result == false then
    local err_msg = select(2, ...)
    print(debug.traceback(co, err_msg))
  end
  return result
end

local function doRequest(req, dest)
  print("In lua doRequest, req:", req, ", dest", dest)
  local s = genSession()
  local co = coroutine.create(function (request, session, reqdest)
      print("In coroutine", session, ", req:", request)
      local resp, sess, respdest = c.call(request, session, reqdest)
      print("Coroutine", sess, "resume", ", resp:", resp, "dest:", respdest)
      c.send(resp, respdest)
      print("Coroutine", sess, "end.")
  end)
  local result = handleCoroutineResumeResult(co, coroutine.resume(co, req, s, dest))
  if result then
    tCoroutine[s] = co
  end
  print("lua doRequest end, req:", req)
end

local function doResponse(resp, session, dest)
  print("In lua doResponse, resp:", resp, ", session", session, ", dest", dest)
  local co = tCoroutine[session]
  if co == nil then
      print("No coroutine correspond session:", session)
  else
      handleCoroutineResumeResult(co, coroutine.resume(co, resp, session, dest))
      tCoroutine[session] = nil
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



getRequest = "GET /getrequest.txt HTTP/1.1\r\n"
getRequest+="Host: [rsid].112.2o7.net\r\n"
getRequest+="Keep-Alive: timeout=15\r\n"
getRequest+="Connection: Keep-Alive\r\n"
getRequest+="X-Forwarded-For: 192.168.10.1\r\n\r\nHere's some entity body content!"
# getRequest+="\r\n"
# getRequest+="\r\n"


curCache = getRequest
# "GET url HTTP/1.1" + '\r\n' + "Here's the rest of the message!"

msglist = curCache.split('\r\n')

request = msglist[0].split()

get = request[0]
filepath = request[1]
version = request[2]
headers = {}
brokeAtIndex = -1
for x in range(1, len(msglist)):
	if msglist[x] == '\r\n':
		brokeAtIndex = x
		break
	elif msglist[x].find(': ') > -1:
		msg = msglist[x].split(': ')
		headers[msg[0]] = msg[1]
	else:
		break;
entityBody = msglist[brokeAtIndex]
# for x in range(brokeAtIndex + 1,len(msglist)):
# 	entityBody+=msglist[x]

# handle entity body
print get, '-', filepath, '-', version
print "HEADERS: \n", headers
print "Entity Body: \n",entityBody 
# print "", msglist
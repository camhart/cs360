import select, socket, sys, time, errno, os
from email.utils import formatdate

class Poller:
    """ Polling server """
    def __init__(self,port=8080, debug=False):
        self.host = ""
        self.port = port
        self.open_socket()
        self.clientId = set()
        self.clients = {}
        self.clientsActivity = {}
        self.clientsCache = {}
        self.hosts = {}
        self.media = {}
        self.parameters = {}
        self.size = 10240
        self.cacheMax = 40960
        self.debug = debug
        self.sweepInterval = 1
        self.get = None
        self.filepath = None
        self.version = None
        self.headers = {}
        self.notImplemented = ["POST", "HEAD", "OPTIONS", "PUT", "DELETE", "TRACE", "CONNECT", ""]

    def open_socket(self):
        """ Setup the socket for incoming clients """
        try:
            self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR,1)
            self.server.bind((self.host,self.port))
            self.server.listen(5)
        except socket.error, (value,message):
            if self.server:
                self.server.close()
            print "Could not open socket: " + message
            sys.exit(1)

    def run(self):
        self.parseConfig()
        """ Use poll() to handle each incoming client."""
        if self.debug:
            print "Server started..."
        self.poller = select.epoll()
        self.pollmask = select.EPOLLIN | select.EPOLLHUP | select.EPOLLERR
        self.poller.register(self.server,self.pollmask)

        self.lastSweep = time.time()
        while True:
            # poll sockets
            #store current time
            self.sweep()
            try:
                fds = self.poller.poll(timeout=1)
            except:
                return
            #self.sweep()
            #current time, find difference
            for (fd,event) in fds:
                # handle errors
                if event & (select.POLLHUP | select.POLLERR):
                    self.handleError(fd)
                    continue
                # handle the server socket
                if fd == self.server.fileno():
                    self.handleServer()
                    continue
                # handle client socket
                result = self.handleClient(fd)

    def sweep(self):
        if time.time() - self.lastSweep > self.sweepInterval:
            fdsToRemove = set()
            for x in self.clientId:
                if time.time() - self.clientsActivity[x] > self.parameters["timeout"]:
                    fdsToRemove.add(x)           
            while len(fdsToRemove) > 0:
                self.cleanupClient(fdsToRemove.pop())        
            self.lastSweep = time.time()

    def cleanupClient(self, fd):
        self.clients[fd].close()
        del self.clients[fd]
        del self.clientsCache[fd]
        del self.clientsActivity[fd]
        self.clientId.remove(fd)
        self.poller.unregister(fd)

    def handleError(self,fd):
        if fd == self.server.fileno():
            self.poller.unregister(fd)            
            # recreate server socket
            self.server.close()
            self.open_socket()
            self.poller.register(self.server,self.pollmask)
        else:
            # close the socket
            self.cleanupClient(fd)

    def handleServer(self):
        (client,address) = self.server.accept()
        client.setblocking(0)
        self.clients[client.fileno()] = client
        self.clientsActivity[client.fileno()] = time.time()
        self.clientsCache[client.fileno()] = ""
        self.poller.register(client.fileno(),self.pollmask)

    def parseRequest(self, getRequest):
        if self.debug:
            print getRequest
        msglist = getRequest.strip().split('\r\n')
        request = msglist[0].split()
        self.get = request[0]
        self.filepath = request[1]
        if self.filepath == "/":
            self.filepath = "/index.html"
        self.version = request[2]
        self.headers = {}
        brokeAtIndex = -1
        for x in range(1, len(msglist)):
            if msglist[x] == '\r\n':
                brokeAtIndex = x
                break
            elif msglist[x].find(': ') > -1:
                msg = msglist[x].split(': ')
                self.headers[msg[0]] = msg[1]
            else:
                break;
        self.entityBody = msglist[brokeAtIndex]

    def getFilePath(self, filepath):
        host = self.headers["Host"]
        index = host.find(":")
        if index > -1:
            host = host[0:index]
        return os.getcwd() + self.getHost().rstrip("/") + filepath;        

    def getHost(self):
        host = self.headers["Host"]
        ret = ""
        try:
            ret = self.hosts[host]
        except KeyError, e:
            ret = self.hosts["default"]
        return ret

    def getMediaType(self, filepath):
        fileExt = filepath.split('.')[1]
        return self.media[fileExt]

    def fileAccess(self, filepath):
        try:
            open(filepath)
        except IOError as (errno, strerror):
            if errno == 13:
                return (False, 403, "<html><body><h1>403 - Forbidden</h1></body></html>")
            elif errno == 2:
                return (False, 404, "<html><body><h1>404 - Not Found</h1></body></html>")
            else:
                return (False, 500, "<html><body><h1>500 - Internal Server Error</h1></body></html>")
        return (True, 200, None)

    def sendFile(self, clientSock, filepath, bytessent=-1, metabytes=-1):
        if metabytes > -1:
            totalbytes = os.stat(filepath).st_size #+ metabytes
        totalsent = 0 #bytessent
        with open(filepath) as fileObj:
            newChunk = True
            while True:
                try:
                    # if metabytes > -1:
                    #     print totalsent," / ",totalbytes
                    if newChunk:
                        chunk = fileObj.read(51200)
                    newChunk = True
                    # if not chunk:
                    chunkSize = len(chunk)
                    if not chunk:
                        # if metabytes > -1:
                        #     print totalsent," / ",totalbytes
                        # print "not chunk..."
                        break
                    bytesSent = clientSock.send(chunk)
                    totalsent+=bytesSent
                    if not(bytesSent == chunkSize):
                        newChunk = False
                        chunk = chunk[bytesSent:]
                except socket.error, e:
                    if e[0] in (errno.EWOULDBLOCK, errno.EAGAIN):
                        newChunk = False
                        continue
                    else:
                        msg = self.buildResponseMsg(data="<html><body><h1>500 - Internal Service Error</h1></body></html>", code=500, sendData=True)
                        # success = self.clients[clientfd].sendall(msg)
                        self.sendMsg(clientSock, msg)
                        break                


    def getFileData(self, fp):
        data = ""
        with open(fp) as f:
            data = f.read()
        return data

    def handleMessage(self, clientfd, data):
        self.parseRequest(data)
        # print data
        truefp = self.getFilePath(self.filepath)        
        # print truefp
        (canAccess, code, err) = self.fileAccess(truefp)
        if not(self.get == "GET"):
            if self.get in self.notImplemented:
                canAccess = False
                code = 501
                err = "<html><body><h1>501 - Not Implemented</h1></body></html>"
            else:
                canAccess = False
                code = 400
                err = "<html><body><h1>400 - Invalid Request</h1></body></html>"
        msg = ""
        #add while loop to ensure the send buffer isn't full.
        while True:
            try:
                if not(canAccess):
                    msg = self.buildResponseMsg(err, code, sendData=True)
                    # success = self.clients[clientfd].sendall(msg)
                    success = self.sendMsg(self.clients[clientfd], msg)
                else:
                    # fdata = self.getFileData(truefp)
                    size = os.stat(truefp).st_size
                    msg = self.buildResponseMsg(data=size, code=code, lastModified=self.getLastModified(), sendData=False)
                    # success = self.clients[clientfd].send(msg)
                    # print msg

                    bsent = self.sendMsg(self.clients[clientfd], msg)
                    self.sendFile(clientSock=self.clients[clientfd], filepath=truefp, bytessent=bsent, metabytes=len(msg))
                # print msg
                break
            except socket.error, e:
                if e[0] in (errno.EWOULDBLOCK, errno.EAGAIN):
                    print "socket error on send?"
                    continue
                else:
                    msg = self.buildResponseMsg(data="<html><body><h1>500 - Internal Service Error</h1></body></html>", code=500, sendData=True)
                    # success = self.clients[clientfd].sendall(msg)
                    self.sendMsg(self.clients[clientfd], msg)
                    break

    def getHTTPDate(self, mtime=-1):
        if mtime == -1:
            mtime = time.time()
        # gmt = time.gmtime(mtime)
        # format = '%a , % d % b % Y % H :% M :% S GMT'
        # time_string = time.strftime ( format , gmt )
        time_string = formatdate(timeval = mtime, localtime = False, usegmt = True)
        return time_string

    def sendMsg(self, client, msg):
        if self.debug:
            print msg
        totalbytes = len(msg)
        bytessent = 0
        newChunk = True
        while True:
            try:
                if bytessent == totalbytes:
                    break
                bs = client.send(msg[bytessent:totalbytes])
                if not(bs == len(msg[bytessent:totalbytes])):
                    # print "messup, ",bs
                    pass
                else:
                    bytessent+=bs

            except socket.error, e:
                if e[0] in (errno.EWOULDBLOCK, errno.EAGAIN):
                    # print "socket error on send? inside!"
                    # time.sleep(1)
                    continue
                else:
                    # print "poopy..."
                    msg = self.buildResponseMsg(data="<html><body><h1>500 - Internal Service Error</h1></body></html>", code=500, sendData=True)
                    success = self.clients[clientfd].sendall(msg)
                    return -1
                    break   
        return bytessent


    def getLastModified(self, mfilepath=""):
        if mfilepath == "":
            mfilepath = self.filepath
        return os.stat(self.getFilePath(mfilepath)).st_mtime

    def buildResponseMsg(self, data, code=200, lastModified=-1, sendData=True):
        response = "HTTP/1.1 "
        response += str(code)
        if code == 200:
            response += " OK"
        elif code == 400:
            response += " Bad Request"
        elif code == 403:
            response += " Forbidden"
        elif code == 404:
            response += " Not Found"
        elif code == 500:
            response += " Internal Server Error"
        elif code > 500:
            response += " Not Implemented"
        response += "\r\n"
        response += "Date: "
        response += self.getHTTPDate()
        response += "\r\n"
        response += "Server: CamHartPythonWebServer/0.0\r\n"
        response += "Content-Type: "
        if code == 200:
            index = self.filepath.rfind('.')
            response += self.media[self.filepath[index:].lstrip('.').strip()]
        else:
            response += "text/html"
        response += "\r\n"
        response += "Content-Length: "
        if sendData:
            response += str(len(data))
        else:
            response += str(data)
        if code == 200:
            # response += "\r\n"
            response += "\r\nLast-Modified: "
            if lastModified == -1:
                response += self.getHTTPDate()
            else:
                response += self.getHTTPDate(lastModified)
        response += "\r\n\r\n"
        if sendData:
            response += data
        return response

    def parseConfig(self):
        data = ""
        with open(os.getcwd() + "/web.conf") as f:
            data = f.read()
        lines = data.split('\n')
        for x in range(0, len(lines)):
            if lines[x][0:5] == "host ":
                line = lines[x].split()
                self.hosts[line[1].strip()] = line[2].strip()
            elif lines[x][0:6] == "media ":
                line = lines[x].split()
                self.media[line[1].strip()] = line[2].strip()                
            elif lines[x][0:10] == "parameter ":
                line = lines[x].split()
                self.parameters[line[1].strip()] = int(line[2].strip())
            else:
                if not(lines[x].strip() == ""):
                    print "Invalid web.conf file - line(",x,")\n",lines[x]
                    sys.exit(0)
                else:
                    continue
        self.sweepInterval = self.parameters["timeout"] / 2

    def handleClient(self,fd):
        while True:
            try:
                if fd not in self.clientId:
                    self.clientId.add(fd)
                data = self.clients[fd].recv(self.size)
                if not data:
                    break
                self.clientsActivity[fd] = time.time()
                self.clientsCache[fd]+= data
                curCache = self.clientsCache[fd]    
                index = curCache.find('\r\n\r\n') + 3

                if index > -1:
                    # complete message found
                    curMsg = curCache[0:index]

                    self.clientsCache[fd] = curCache[index + 1:]
                    while True:
                        if curMsg.strip() == "":
                            break

                        self.handleMessage(fd, curMsg)

                        curCache = self.clientsCache[fd]
                        index = curCache.find('\r\n\r\n') + 3 
                        if index == -1:
                            break;
                        curMsg = curCache[0:index]
                        self.clientsCache[fd] = curCache[index + 1:]   
                    #>>>
                    # "Then the server should continue to call recv() for that client until it either receives a complete message or the socket returns an error code indicating that no data is available. Your server should handle all messages it has received so far and then move on to the next client."
                    # self.cleanupClient(fd)
                    break      
                # else:
                #     break           
                # else:
                #     self.cleanupClient(fd)
                #     break
            except socket.error, e:
                if e[0] in (errno.EWOULDBLOCK, errno.EAGAIN):
                    break;
                else:
                    print str(e)
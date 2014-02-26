import argparse
import os
import requests
import threading
import time

''' Downloader for a set of files '''
class Downloader:
    def __init__(self):
        ''' initialize the file where the list of URLs is listed, and the
        directory where the downloads will be stored'''
        self.args = None
        self.in_file = 'urls.txt'
        self.parse_arguments()

    def parse_arguments(self):
        parser = argparse.ArgumentParser(prog='Download Accelerator', description='Multi threaded download accelerator', add_help=True)
        parser.add_argument('-n', '--num', type=int, action='store', help='Specify the number of threads to use when downloading')
        parser.add_argument("URL", type=str, action='store', help='URL to download')
        args = parser.parse_args()

        self.threadCount = args.num
        self.url = args.URL
        #print "Threads: ", self.threadCount
        #print "Url: ",  self.url

    def download(self):

        self.fileName = self.url.split('/')[-1]
        if len(self.fileName) == 0:
            self.fileName = "/index.html"
            self.url+=self.fileName[1:]

        try :
            r = requests.head(self.url)
            self.fileSize = int(r.headers['content-length'])
        except KeyError:
            print "No valid content-length header found for", self.url
            return

        self.dthreads = []
        curStartPos = 0;
        incr = int(self.fileSize / self.threadCount)
        #threads are created here, start the timer...
        self.startTime = time.time()
        for c in range(0, self.threadCount):
            if c == self.threadCount - 1:
                self.dthreads.append(DownThread(self.url, self.fileName, curStartPos, self.fileSize - 1))
                #print curStartPos, self.fileSize, self.fileSize - curStartPos
            else:
                self.dthreads.append(DownThread(self.url, self.fileName, curStartPos, curStartPos + incr - 1))
                #print curStartPos, curStartPos + incr - 1
            
            curStartPos += incr

        for t in self.dthreads:
           t.start()
        with open('./' + self.fileName, 'wb') as f:
            for t in self.dthreads:
                t.join()
                f.write(t.content)
        #print the output
        print self.url, self.threadCount, self.fileSize, time.time() - self.startTime


''' Use a thread to download one file given by url and stored in filename'''
class DownThread(threading.Thread):
    def __init__(self,url,filename,startVal, endVal):
        self.url = url
        #self.filename = filename
        self.startVal = startVal
        self.endVal = endVal
        threading.Thread.__init__(self)
        self._content_consumed = False

    def run(self):
        byteRange = "bytes="
        byteRange+=str(self.startVal)
        byteRange+="-"
        if self.endVal != -1:
            byteRange+=str(self.endVal)
        #print byteRange
        r = requests.get(self.url, headers={"Range":byteRange}, stream=True)
        #print r
        try:
            contentRange = r.headers['content-range']
        except:
          print "No response content-range provided"
        self.content = r.content
 
if __name__ == '__main__':
    d = Downloader()
    d.download()
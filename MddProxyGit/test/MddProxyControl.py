#!/usr/bin/python

import sys
import argparse


class MddProxyClient(object):
    def __init__(self,sock):
	if sock==None:
	       self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        else:
            self.sock = sock
            
    def connect(self,host,port):
	   self.sock.connect(host,port)

    def sendCommand(self, msg):
	   sent = self.sock.send(msg)
           if sent == 0:
               raise RuntimeError("socket connection broken")
    
    def receiveResponse(self):
        chunks = []
        bytes_recd = 0
        while bytes_recd < MSGLEN:
            chunk = self.sock.recv(min(MSGLEN - bytes_recd, 2048))
            if chunk == b'':
                raise RuntimeError("socket connection broken")
            chunks.append(chunk)
            bytes_recd = bytes_recd + len(chunk)
        return b''.join(chunks)



def main(argv):
    parser = argparse.ArgumentParser(description='Mdd Proxy control client')
    parser.add_argument('--host', default="127.0.0.1", type=int, help='mgmt port/control port of Mdd proxy');
    parser.add_argument('--port', default=6800, type=int, help='mgmt port/control port of Mdd proxy');
    parser.add_argument('--stop', help='stop mdd proxy')

    args = parser.parse_args()
    print args
    
    proxyClient = MddProxyClient(None)
    proxyClient.connect(args.host,args.port)
    
    if args.stop==None:
        proxyClient.sendCommand(stop)


     

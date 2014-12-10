import zmq
c = zmq.Context()
s = c.socket(zmq.SUB)
s.connect('tcp://protophone.local:12345')
s.setsockopt(zmq.SUBSCRIBE, b'24')
s.setsockopt(zmq.SUBSCRIBE, b'23')
while True:
	print s.recv()

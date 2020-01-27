
from zio.flow import objectify

def message_to_dict(msg):
    '''
    Return a simple dictionary of message header info.
    '''
    d = objectify(msg)
    d['origin'] = msg.origin
    d['granule'] = msg.granule
    d['seqno'] = msg.seqno
    return d


AF_INET = 2
AF_INET6 = 10
AF_UNSPEC = 0
SOCK_STREAM = 1
SOCK_DGRAM = 2
IPPROTO_TCP = 6
IPPROTO_UDP = 17
AI_PASSIVE = 1
AI_CANONNAME = 2
AI_NUMERICHOST = 4
AI_NUMERICSERV = 8
SOL_SOCKET = 1
SO_REUSEADDR = 2
SOCK_NONBLOCK = 2048
SOCK_CLOEXEC = 524288
SHUT_RDWR = 2

# Commonly accessed sentinel in urllib, http.client, etc.
_GLOBAL_DEFAULT_TIMEOUT = object()

# Exceptions
error = OSError
timeout = TimeoutError
gaierror = OSError
herror = OSError

# Dummy functions
def getaddrinfo(*args, **kwargs):
    return [(AF_INET, SOCK_STREAM, IPPROTO_TCP, '', ('127.0.0.1', 0))]

def getnameinfo(*args, **kwargs):
    return ('localhost', '0')

def gethostname():
    return 'localhost'

def gethostbyname(name):
    return '127.0.0.1'

def getfqdn(name=None):
    return 'localhost'

def create_connection(*args, **kwargs):
    raise NotImplementedError("Networking disabled")

def fromfd(*args, **kwargs):
    raise NotImplementedError("Networking disabled")

def socketpair(*args, **kwargs):
    raise NotImplementedError("Networking disabled")

# Dummy socket class
class socket:
    def __init__(self, *args, **kwargs):
        raise NotImplementedError("Dummy socket: networking disabled")

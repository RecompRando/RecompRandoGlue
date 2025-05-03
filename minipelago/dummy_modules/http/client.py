# Dummy http.client module to satisfy imports without real HTTP functionality

class HTTPConnection:
    def __init__(self, *args, **kwargs):
        raise NotImplementedError("http.client.HTTPConnection is not supported")

    def request(self, *args, **kwargs):
        raise NotImplementedError()

    def getresponse(self):
        raise NotImplementedError()

class HTTPResponse:
    def __init__(self, *args, **kwargs):
        raise NotImplementedError()

class HTTPSConnection(HTTPConnection):
    pass

class HTTPException(Exception):
    pass

class NotConnected(HTTPException):
    pass

class InvalidURL(HTTPException):
    pass

class ImproperConnectionState(HTTPException):
    pass

class ResponseNotReady(HTTPException):
    pass

class CannotSendRequest(HTTPException):
    pass

class CannotSendHeader(HTTPException):
    pass

class LineTooLong(HTTPException):
    pass

class RemoteDisconnected(HTTPException):
    pass

# Constants
HTTP_PORT = 80
HTTPS_PORT = 443

# Dummy asyncio module to satisfy imports without real async support

def run(*args, **kwargs):
    raise NotImplementedError("asyncio is not supported in this build")

class Future:
    def __init__(self, *args, **kwargs):
        raise NotImplementedError()

class Task(Future):
    pass

class AbstractEventLoop:
    def run_forever(self): raise NotImplementedError()
    def run_until_complete(self, future): raise NotImplementedError()
    def stop(self): raise NotImplementedError()

def get_event_loop():
    raise NotImplementedError()

def new_event_loop():
    raise NotImplementedError()

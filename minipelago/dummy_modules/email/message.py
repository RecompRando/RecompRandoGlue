# Dummy email module to satisfy imports without real email functionality

class Message:
    def __init__(self, *args, **kwargs):
        raise NotImplementedError("email.Message is not supported in this build")

    def set_payload(self, *args, **kwargs):
        raise NotImplementedError()

    def add_header(self, *args, **kwargs):
        raise NotImplementedError()

def message_from_string(*args, **kwargs):
    raise NotImplementedError("email.message_from_string is not supported")

def message_from_file(*args, **kwargs):
    raise NotImplementedError("email.message_from_file is not supported")

def mime_type(*args, **kwargs):
    raise NotImplementedError("email.mime_type is not supported")

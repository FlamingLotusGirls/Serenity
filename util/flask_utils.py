from flask import Response

class CORSResponse(Response):
    def __init__(self, status_str, status_code, mimetype="text/plain"):
        super().__init__(status_str, status_code, mimetype=mimetype)
        self.headers['Access-Control-Allow-Origin'] = '*'


class JSONResponse(CORSResponse):
    def __init__(self, status_str, status_code=200):
        super().__init__(status_str, status_code, "application/json")


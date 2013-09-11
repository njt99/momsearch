import BaseHTTPServer
import shutil
import cgi

class MomRequestHandler(BaseHTTPServer.BaseHTTPRequestHandler):
	"""HTTP request handler for mom search."""

	def do_POST(self):
		"""Server a POST request."""
		self.send_response(200)
		self.send_header('Content-type', 'text/html')
		self.end_headers()
		form = cgi.FieldStorage(fp=self.rfile, headers=self.headers,
					environ={'REQUEST_METHOD':'POST', 'CONTENT_TYPE':self.headers['Content-Type']})
		if path == '/next':
			do_next()
		elif path == '/submit':
			do_submit()

	def do_next():
		"""Reserve and return the next unit of work."""

	def do_submit():
		"""Sumit the results of a unit of work."""

if  __name__ == '__main__':
	BaseHTTPServer.test(MomRequestHandler, BaseHTTPServer.HTTPServer)

import BaseHTTPServer
import shutil
import cgi
import re
import sys
import json

class MomRequestHandler(BaseHTTPServer.BaseHTTPRequestHandler):
    """HTTP request handler for mom search.

    URLS:
        /next (POST): Reserve the next box which needs work.
        /submit (POST): Submit the results for a box.

        /tree/<code>: Get a part of the input tree.
        /args: Get the arguments to be used for refine.
        /words: Get the words to be tried all the time.
        /powers: Get the word powers data.
        /moms: Get the MOM words.
        /parameterized: Get the parameterized words.
    """

    queue = {}
    config_file = 'mom.config'

    def __init__(self, *args, **kwargs):
        if len(sys.argv) > 1:
            self.config_file = sys.argv[1]

        with open(self.config_file,'r') as config_fp:
            config = json.loads(config_fp.read())
            self.words_file = config['words_file']
            self.powers_file = config['powers_file']
            self.moms_file = config['moms_file']
            self.parameterized_file = config['parameterized_file']
            self.src_dir = config['src_dir']
            self.treecar = config['treecat']

        BaseHTTPServer.BaseHTTPRequestHandler.__init__(self, *args, **kwargs) 

    def do_POST(self):
        """Serve a POST request."""
        form = cgi.FieldStorage(fp=self.rfile, headers=self.headers,
                    environ={'REQUEST_METHOD':'POST', 'CONTENT_TYPE':self.headers['Content-Type']})
        if self.path == '/next':
            do_next(form)
        elif self.path == '/submit':
            do_submit(form)

    def do_next(self, form):
        """Reserve and return the next unit of work."""
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        # TODO(njt): implement


    def do_submit(self, form):
        """Submit the results of a unit of work."""
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        # TODO(njt): implement
    
    def write_file(self, file):
        with open(file,'r') as fp:
            data  = fp.read()
            if data:
                self.wfile.write(data)

    def do_GET(self):
        """Serve a GET request."""
        match = re.match('/tree/[01]*', self.path)
        if match:
            tree_data = subprocess.check_output([self.treecat, self.src_dir, match.group(1)])
            self.write_file(tree_data)
        if self.path == '/args':
            self.write_file('-m 36 -t 6 -i 36 -s 3000000 -a 5.1')
        if self.path == '/words':
            self.write_file(self.words_file)
        if self.path == '/powers':
            self.write_file(self.powers_file)
        if self.path == '/moms':
            self.write_file(self.moms_file)
        if self.path == '/parameterized':
            self.write_file(self.parameterized_file)

            
        


if  __name__ == '__main__':
    BaseHTTPServer.test(MomRequestHandler, BaseHTTPServer.HTTPServer)

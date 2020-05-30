#!/usr/bin/env python3.7.4

"""
    Handlers for the http server implemented here

    @Author     Emil Stubbe Kolvig-Raun
    @Contact   emstu15@student.sdu.dk

    @Author     Morten Abrahamsen Olesen
    @Contanct   moole15@student.sdu.dk

"""

import asyncio, jinja2, os, base64
from aiohttp import web

class HTTPServer:

    def __init__(self, config, routes):
        self.config = config                   # configuration file (/../etc/)
        self.routes = routes                   # web route handler
        self.host = config['http']['host']     # the host which the webserver is running on
        self.port = config['http']['port']     # and the port to access it

        self.application = web.Application()    # app as class var
        self.loop = asyncio.new_event_loop()    # asyncio loop (for numerous threads simultaniously)

    def run(self):
        from aiohttp_session import setup
        from aiohttp_session.cookie_storage import EncryptedCookieStorage
        from cryptography.fernet import Fernet
        from aiohttp_jinja2 import setup as jinja_setup

        asyncio.set_event_loop(self.loop)

        # Setting up symmetric cookie encryption using  
        # Fernet that guarantees that a message encrypted
        # using it cannot be manipulated or read without the key
        setup(self.application, EncryptedCookieStorage(base64.urlsafe_b64decode(Fernet.generate_key())))

        # Setting up the template engine and loading 
        # any and every content from the html folder
        jinja_setup(self.application, loader=jinja2.FileSystemLoader(os.path.join((os.path.split(__file__))[0],"html")))
        
        # Adding the different routes to the webserver
        # anything from pages must be added to (path to page, handler)
        self.application.add_routes(self.routes)  

        # referenced to using /static/
        self.application.router.add_static('/style/', path='./html/style/', name='styles')
        self.application.router.add_static('/images/', path='./html/img/', name='images')
        self.application.router.add_static('/', path='./html/icons/', name='favicon')

        # Running the webserver using the host
        # and port configurations, and the web application
        server = self.loop.create_server(self.application.make_handler(), self.host, self.port)

        print('')
        print('========== HTTP server running on: http://%s:%d/ ==========' % (self.host, self.port))

        # using asyncio to gather the processes as tasks and run 
        # them separately in foreground and background

        return self.loop, asyncio.gather(server)
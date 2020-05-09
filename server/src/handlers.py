#!/usr/bin/env python3.7.4

"""
    Handlers for the http server implemented here

    @Author     Emil Stubbe Kolvig-Raun
    @Contanct   emstu15@student.sdu.dk

    @Author     Morten Abrahamsen Olesen
    @Contanct   moole15@student.sdu.dk

"""

from aiohttp import web
from aiohttp_jinja2 import render_template

class Handler:

    def __init__(self):    

        self.routes = [ 
            web.get('/', self.main_page),
            web.get('/configuration', self.config_page),
            web.get('/livedata', self.live_page),
            web.get('/ventilation', self.ventilation_page),

            # page routes
            # GET
            
            # PUT
        ]
        
    # this page provides the developer with an overview of the API
    # returns the index.html file fom /static/html
    async def main_page(self, request):
        context = {}
        response = render_template("index.html", request, context)
        response.headers['Content-Language'] = 'en'
        return response

    async def config_page(self, request):
        context = {}
        response = render_template("config.html", request, context)
        response.headers['Content-Language'] = 'en'
        return response
    
    async def live_page(self, request):
        context = {}
        response = render_template("live.html", request, context)
        response.headers['Content-Language'] = 'en'
        return response
    
    async def ventilation_page(self, request):
        context = {}
        response = render_template("ventilation.html", request, context)
        response.headers['Content-Language'] = 'en'
        return response

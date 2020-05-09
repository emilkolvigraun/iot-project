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
import common 

class Handler:

    def __init__(self, com:common.Common):    
        self.com = com
        self.routes = [ 
            web.get('/', self.main_page),
            web.get('/configuration', self.config_page),
            web.get('/livedata', self.live_page),
            web.get('/rooms', self.rooms_page),

            # configuration routes
            web.get('/configuration/sensor/get', self.get_sensors),
            web.post('/configuration/sensor/set/room', self.set_sensor_configuration)

        ]
        
    # this page provides the developer with an overview of the API
    # returns the index.html file fom /static/html
    async def main_page(self, request):
        context = {}
        response = render_template("index.html", request, context)
        response.headers['Content-Language'] = 'en'
        return response

    # this page provides the user with configuration options
    # returns the config.html file fom /static/html
    async def config_page(self, request):
        context = {}
        response = render_template("config.html", request, context)
        response.headers['Content-Language'] = 'en'
        return response
    
    # this page provides the user with live data
    # returns the live.html file fom /static/html
    async def live_page(self, request):
        context = {}
        response = render_template("live.html", request, context)
        response.headers['Content-Language'] = 'en'
        return response
    
    # this page provides the user with live data and settings on ventilation
    # returns the ventilation.html file fom /static/html
    async def rooms_page(self, request):
        context = {}
        response = render_template("rooms.html", request, context)
        response.headers['Content-Language'] = 'en'
        return response

    # returns the registered sensors
    async def get_sensors(self, request):
        return web.Response(text=str(list(self.com.sensors.keys())))

    async def set_sensor_configuration(self, request):

        payload = await request.json()

        print(payload)
        
        return web.Response(status=200)
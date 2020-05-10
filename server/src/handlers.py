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
import common, json

class Handler:

    def __init__(self, com:common.Common, users):    
        self.com = com
        self.users = users
        self.routes = [ 
            web.post('/login', self.login),
            web.get('/', self.main_page),
            web.get('/configuration', self.config_page),
            web.get('/livedata', self.live_page),
            web.get('/rooms', self.rooms_page),

            # configuration routes
            web.get('/configuration/sensor/get', self.get_sensors),
            web.get('/configuration/rooms/get', self.get_rooms),
            web.get('/configuration/rooms/get/configuration/{room}', self.get_room_configuration),
            web.delete('/configuration/rooms/delete/configuration/{room}', self.delete_room),
            web.post('/configuration/sensor/set/room', self.set_sensor_configuration)

        ]
        
    async def login(self, request):
        payload = await request.json()
        username = payload['username']
        print(username)
        if username not in list(self.users.keys()):
            return web.Response(status=401) # unauthorized
        elif username in list(self.users.keys()) and payload['password'] == self.users[username]:
            return web.Response(status=200) # unauthorized
        else:
            return web.Response(status=401)

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

    # returns the configured rooms
    async def get_rooms(self, request):
        return web.Response(text=str(list(self.com.rooms.keys())))

    # returns the configured rooms
    async def get_room_configuration(self, request):
        room = request.match_info.get('room', '')
        configuration = {}
        if room in self.com.rooms.keys():
            configuration = self.com.rooms[room]
        payload = json.dumps(configuration)
        return web.Response(text=payload)

    async def set_sensor_configuration(self, request):
        payload = await request.json()
        self.com.append_room(payload)
        self.com.update(common.PUBLISH, payload['sensor']+'/room/config', json.dumps(payload))
        return web.Response(status=200)

    async def delete_room(self, request):
        room = str(request.match_info.get('room', ''))
        try:
            del self.com.rooms[room]
        except:
            pass
        return web.Response(status=200)
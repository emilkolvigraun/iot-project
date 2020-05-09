#!/usr/bin/env python3.7.4

"""
    Handlers for the http server implemented here

    @Author     Emil Stubbe Kolvig-Raun
    @Contanct   emstu15@student.sdu.dk

    @Author     Morten Abrahamsen Olesen
    @Contanct   moole15@student.sdu.dk

"""

def load_configuration():
    # importing jso
    import json

    # Importing the configuration file
    # holds information about which host 
    # and port to run the server on
    with open('../etc/config.json') as cf:
        config = json.loads(''.join(cf.readlines()))

    return config


if __name__ == "__main__":

    # as the method says...
    config = load_configuration()
    
    from loop import Loop
    looper = Loop(config)

    # importing the handler
    from handlers import Handler

    # initializing the handler
    # includes all callbacks
    handler = Handler(looper.com)
    
    # importing the server implmentation
    from server import HTTPServer

    # Starting the webserver, passing in all 
    # route definitions
    http_server = HTTPServer(config, handler.routes)
    
    
    try:

        # start the processes
        looper.start()    
        loop, tasks = http_server.run()
        
        loop.run_until_complete(tasks)
        loop.run_forever()

    except KeyboardInterrupt:
        looper.stop()
        exit('=================== Server Shutting Down... ====================')
    except Exception as e:
        exit('Exited because of error: %s'%(e))


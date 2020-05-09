#!/usr/bin/env python3.7.4

"""
    Handlers for the http server implemented here

    @Author     Emil Stubbe Kolvig-Raun
    @Contanct   emstu15@student.sdu.dk

    @Author     Morten Abrahamsen Olesen
    @Contanct   moole15@student.sdu.dk

"""

import time

class Log:
    folder: str = 'logs/'
    file: str = 'log_1'
    logs: list = []

    def log(self, tag:str, msg:str):
        self.logs.append((tag, msg))

        # make sure not to write to the file all the time
        # save the power and do it in bursts
        # if len(self.logs) >= 1:
        self.__write()

    def finish(self):
        self.__write()

    def __getfile(self):
        try:
            f = open(self.folder+self.file+'.txt', 'r')
        except:
            self.__created_new()
            f = open(self.folder+self.file+'.txt', 'w')

        file_size = f.__sizeof__()
        f.close()
        
        # create new file if size above 100 mb
        if file_size > 100000:
            self.log('LOGGGER', 'File size > 100MB')

            self.file = 'log_'+str(int(self.file.split('_')[1])+1)

            tf = open(self.folder+self.file+'.txt', 'w')
            tf.close()

            self.__created_new()

        return self.folder+self.file+'.txt'

    def __created_new(self):
        self.log('LOGGGER', 'Created new log:%s'%self.file)

    def __write(self):
        with open(self.__getfile(), 'a') as f:
            for l in self.logs:
                f.write(str(int(time.time()))+' | '+str(l[0])+' | '+str(l[1])+'\n')
        self.logs.clear()
    

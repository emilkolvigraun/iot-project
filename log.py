
import time

class Log:
    folder: str = 'logs/'
    file: str = 'log_1'
    logs: list = []

    def log(self, tag:str, msg:str):
        self.logs.append((tag, msg))
        # if len(self.logs) >= 1:
        self.__write()

    def finish(self):
        self.__write()

    def __getfile(self):
        f = open(self.folder+self.file+'.txt', 'r')
        sz = f.__sizeof__()
        f.close()
        if sz > 100000:
            self.file = 'log_'+str(int(self.file.split('_')[1])+1)
        return self.folder+self.file+'.txt'

    def __write(self):
        with open(self.__getfile(), 'w') as f:
            for l in self.logs:
                f.write(str(int(time.time()))+' | '+str(l[0])+' | '+str(l[1])+'\n')
        self.logs.clear()
    

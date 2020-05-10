import sqlite3 as sql

class Database:

    database: sql.connect = sql.connect('archive.db')

    def __init__(self):
        self.cursor = self.database.cursor()

    def create(self, room:str, datatype:str):
        self.cursor.execute("CREATE TABLE "+room+"_"+datatype+" (time,sensor,value)")
        self.cursor.commit()

    def insert(self, room:str, datatype:str, sensor:str, value:str, time:str):
        self.cursor.execute("INSERT INTO"+room+"_"+datatype+" VALUES ("+time+","+sensor+","+value+")")
        self.cursor.commit()

    def get_where(self, from_date, to_date, room:str, datatype:str, sensor:str):
        self.cursor.execute("SELECT value FROM "+room+"_"+datatype+"WHERE sensor=? AND time>=? AND time <=?", (sensor, from_date, to_date, ))

    def table_exists(self, room:str, datatype:str):
        self.cursor.execute("SELECT count(name) FROM sqlite_master WHERE type='table' AND name='"+room+"_"+datatype+"'")

        #if the count is 1, then table exists
        if self.cursor.fetchone()[0]==1 : 
            return True
        return False
        
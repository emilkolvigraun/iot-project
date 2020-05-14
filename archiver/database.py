import sqlite3 as sql

class Database:

    def create(self, table:str):
        with sql.connect('archive.db') as con:
            cursor = con.cursor()
            cursor.execute("CREATE TABLE '"+table+"' (time, datatype, value)")
            con.commit()

    def insert(self, table:str, time:str, datatype:str, value:str):
        with sql.connect('archive.db') as con:
            cursor = con.cursor()
            cursor.execute("INSERT INTO '"+table+"' VALUES ("+time+", '"+datatype+"' ,"+value+")")
            con.commit()

    def get_where(self, from_date, to_date, table:str):
        with sql.connect('archive.db') as con:
            cursor = con.cursor()
            cursor.execute("SELECT value,datatype FROM '"+table+"' WHERE time>="+from_date+" AND time <="+to_date+" ORDER BY time ASC")
            con.commit()
            return cursor.fetchall()

    def table_exists(self, table:str):
        with sql.connect('archive.db') as con:
            cursor = con.cursor()
            cursor.execute("SELECT count(name) FROM sqlite_master WHERE type='table' AND name='"+table+"'")

            #if the count is 1, then table exists
            if cursor.fetchone()[0]>0 : 
                return True
            return False
        
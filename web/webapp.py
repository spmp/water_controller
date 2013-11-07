#!/usr/bin/python3
import bottle
import bottle_sqlite
import glob

app = bottle.Bottle()
sqlite_plugin = bottle_sqlite.SQLitePlugin(dbfile='../log_timeseries.db')
app.install(sqlite_plugin)

def row_as_dict(row):
    return {k: row[k] for k in row.keys()}

@app.get('/')
def index_html():
    return bottle.static_file('index.html', root='.')

@app.get('/favicon.ico')
def index_html():
    return bottle.static_file('/favicon.ico', root='./static')

@app.get('/static/<filename:path>')
def send_static(filename):
    return bottle.static_file(filename, root='./static')

@app.get('/logs/<limit:int>')
def get_logs(limit, db):
    cursor = db.execute('SELECT * FROM data_point ORDER BY time DESC LIMIT ?;',
            (limit,))
    x = list(map(row_as_dict, cursor.fetchall()))
    return { 'values': x }

app.run(host='localhost', port=8080, debug=True, reloader=True)


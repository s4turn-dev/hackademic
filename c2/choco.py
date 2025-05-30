from flask import Flask
from flask import request
import sqlite3
from http import HTTPStatus
from flask import render_template


app = Flask(__name__)

db = sqlite3.connect('HackAdemicBase.db', check_same_thread=False)

db.cursor().execute('''CREATE table if not exists hack(
                        ID integer primary key AUTOINCREMENT,
                        UniqID text,
                        Key string(32)
                        )
                    ''')
# db.cursor().execute("INSERT OR IGNORE INTO hack VALUES(145454, '123', '4adc')")
db.commit()


@app.post('/saveKey')
def save_key():
    cur = db.cursor()
    try:
        cur.execute("INSERT INTO hack (UniqID, Key) VALUES (?,?) ", [request.form['UniqID'], request.form['Key']])
    except KeyError:
        print('S4TURN sent empty message')
        return 'Plaki Plaki', HTTPStatus.BAD_REQUEST
    db.commit()
    return 'Normaldaki', HTTPStatus.OK


@app.get('/ReturnToSender')
def get_key():
    cur = db.cursor()
    try:
        result = cur.execute("SELECT Key FROM hack WHERE UniqID = ? ", [request.form['UniqID']]).fetchone()
    except KeyError:
        print('Chelik ne polychit klyuch')
        return 'Sho delat', HTTPStatus.BAD_REQUEST
    else:
        if result:
            print('4eliky povezlo')
            return result, HTTPStatus.OK
        else:
            print('Takogo 4elika ne sushestvyet')
            return '', HTTPStatus.NOT_FOUND


@app.delete('/DeleteKey')
def delete_key():
    cur = db.cursor()
    try:
        cur.execute("DELETE FROM hack WHERE UniqID = ? ", [request.form['UniqID']])
        db.commit()
    except KeyError:
        print('che ti nesesh?')
        return 'Prover Id', HTTPStatus.BAD_REQUEST
    else:
        print('Vsyo chisto')
        return 'Ok', HTTPStatus.OK


@app.route('/gg')
def xz():
    cur = db.cursor()
    vision = cur.execute('SELECT * FROM hack ')
    return render_template('gg.html', items=vision)


if __name__ == '__main__':
    app.run(host='192.168.100.1')

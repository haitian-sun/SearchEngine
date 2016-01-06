from flask import *
import controllers
from flask.ext.session import Session
from flask.ext.mysqldb import MySQL
import time, os, sys, requests
from werkzeug import secure_filename
from flask_mail import Mail
from flask_mail import Message

app = Flask(__name__)
mysql = MySQL()
app.config['MYSQL_USER'] = 'group6'
app.config['MYSQL_PASSWORD'] = 'group6'
app.config['MYSQL_DB'] = 'group6'
app.config['MYSQL_HOST'] = 'localhost'
mysql.init_app(app)
UPLOAD_FOLDER = 'static/pictures'
ALLOWED_EXTENSIONS = set(['png', 'jpg', 'bmp', 'gif'])
app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER
app.secret_key = 'TPB0D1xHQpAUNBuudltuze7bzekNhCD0pxpG2L3ZkEOOFIXnbKUnIKZEYGiOZ2YB4e' # so secret

app.config.update(dict(
    DEBUG = True,
    MAIL_SERVER = 'smtp.gmail.com',
    MAIL_PORT = 587,
    MAIL_USE_TLS = True,
    MAIL_USE_SSL = False,
    MAIL_USERNAME = 'eecs485fall2015group6@gmail.com',
    MAIL_PASSWORD = 'eecs485group6',
))
mail = Mail(app)

pre = '/ims5p9/pa6'
waiting = 10*60

@app.before_request
def before_request():
	g.db = mysql.connection
	g.pre = pre
	g.waiting = waiting
	g.allowed_file = allowed_file
	g.UPLOAD_FOLDER = UPLOAD_FOLDER
	g.mail = mail

app.register_blueprint(controllers.index,  url_prefix=pre)
#app.register_blueprint(controllers.album,  url_prefix=pre)
#app.register_blueprint(controllers.pic,    url_prefix=pre)
#app.register_blueprint(controllers.user,   url_prefix=pre)
#app.register_blueprint(controllers.search, url_prefix=pre)

def allowed_file(filename):
    return '.' in filename and \
           filename.rsplit('.', 1)[1] in ALLOWED_EXTENSIONS

@app.errorhandler(404)
def not_found(e):
    return "404: Not Found"
    #return render_template('404.html', pre=pre)


if __name__ == "__main__":
	app.run(host = '0.0.0.0', port = 5606, debug = True)


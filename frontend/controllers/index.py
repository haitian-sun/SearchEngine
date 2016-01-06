from flask import *
#from elementtree import *
#from app import mysql
import xml.etree.ElementTree as ET
import time, os, sys
from werkzeug import secure_filename
import collections, urllib, requests
OrderedDict = collections.OrderedDict

index = Blueprint('index', __name__)

@index.route('/')
def index_bp():
	url = g.pre + '/'
	
	query = request.args.get('q')
	w = request.args.get('w')
	if w is not None and query is not None:

		# The ordered dictionary of results
		results = list()

                # Get the results from the IndexServer
                host = "localhost"
                port = 6206

                qstring = urllib.urlencode({"q": str(query).lower(), "w": str(w)})

                req = 'http://' + host + ':' + str(port) + '?' + qstring

                res = requests.get(req)

                jsr = json.loads(res.text)

                cur = g.db.cursor()

                for hit in jsr['hits']:
                    aid = hit['id']

                    stmt = "SELECT * FROM `eecs485_summaries` WHERE eecs485_article_id=" + aid

                    stmt2 = "SELECT eecs485_png_url FROM `eecs485_pa6_images` WHERE eecs485_article_id=" + aid

                    stmt3 = "SELECT eecs485_article_category FROM `eecs485_categories` WHERE eecs485_article_id=" + aid

                    cur.execute(stmt)

                    rv = cur.fetchone()

                    cur.execute(stmt2)

                    rv2 = cur.fetchone()

                    cur.execute(stmt3)

                    rv3 = cur.fetchall()

                    cats = list()
                    for r in rv3:
                        cats.append(r[0])

                    catstring = ", ".join(cats)

                    results.append(OrderedDict([('title', str(rv[1])), ('url', "https://en.wikipedia.org/wiki?curid=" + str(rv[0])), ('details', rv[2]), ('img', str(rv2[0])), ('cats', catstring)]))


		return render_template('index.html', pre=g.pre, url = url, res = True, results = results)

	return render_template('index.html', pre=g.pre, url = url, res = False)

@index.route('/xml')
def xml_parser():
    tree = ET.parse('../data/mining.imageUrls.xml')

    root = tree.getroot()

    cur = g.db.cursor()

    for eecs485_image in root.findall('eecs485_image'):
        id = eecs485_image.find('eecs485_article_id').text
        elt = eecs485_image.find('eecs485_pngs').find('eecs485_png_url')
        if elt is not None and id is not None:
            stmt = "INSERT INTO `eecs485_pa6_images` (eecs485_article_id, eecs485_png_url) VALUES ('" + str(id) + "','" + elt.text + "')"

            cur.execute(stmt)
            print stmt

    g.db.commit()

    return str(root.tag)

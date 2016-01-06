### Details:
  - Before run `./eecs485pa6inv.sh`
       - `cd hadoop-example/mysrc`
       - `make clean`
       - `make `
  - Inverted index: `invIndex.txt`
  - Pagerank:       `pagerank/output`

### Deploy: 
  - Before run `indexServer`
       - `make`
	   - `./indexServer 6206 invIndex.txt`
  - `virtualenv venv6 --distribute`
  - `. venv6/bin/activate`
  - `pip install -r requirements.txt`
  - `cd frontend`
  - `gunicorn -b YOUR_SERVER_NAME app:app`

### Collaborators:
  - Haitian Sun (htsun@umich.edu)
  - Matt Solarz (mssolarz@umich.edu)
  - Tiantong Zhou (tiantong@umich.edu)

### Acknowledgement
  - This is a course project in EECS485 at the University of Michigan. Please do not use it for any other purpose.

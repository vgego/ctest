#!/usr/bin/env python
# -*- coding: utf-8 -*-

import time, os, sys, urllib, urllib2, json
import ssl
ssl._create_default_https_context = ssl._create_unverified_context

os.system("fswebcam --no-banner -r 352x288 /tmp/webcam.jpg")
time.sleep(3)
image_data = open('/tmp/webcam.jpg','rb').read()
#image_data = open(sys.argv[1],'rb').read()

url ='https://aip.baidubce.com/rest/2.0/ocr/v1/general_basic?access_token=xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx'
data = {}
data['language_type'] = "ENG"
#data['image'] = "/9j/4AAQSkZJRgABAQEAYABgAAD/2wBDABMNDxEPDBMREBEWFRMXHTAfHRsbHTsqLSMwRj5KSUU+RENNV29eTVJpU0NEYYRiaXN3fX59S12Jkoh5kW96fXj/2wBDARUWFh0ZHTkfHzl4UERQeHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHh4eHj/wAARCAAfACEDAREAAhEBAxEB/8QAGAABAQEBAQAAAAAAAAAAAAAAAAQDBQb/xAAjEAACAgICAgEFAAAAAAAAAAABAgADBBESIRMxBSIyQXGB/8QAFAEBAAAAAAAAAAAAAAAAAAAAAP/EABQRAQAAAAAAAAAAAAAAAAAAAAD/2gAMAwEAAhEDEQA/APawEBAQEBAgy8i8ZTVV3UY6V1eU2XoWDDZB19S646Gz39w9fkKsW1r8Wm2yo1PYis1be0JG9H9QNYCAgc35Cl3yuVuJZl0cB41rZQa32dt2y6OuOiOxo61vsLcVblxaVyXD3hFFjL6La7I/sDWAgICAgICB/9k="
data['image'] = image_data.encode('base64').replace('\n','')
decoded_data = urllib.urlencode(data)
req = urllib2.Request(url, data = decoded_data)
req.add_header("Content-Type","application/x-www-form-urlencoded")

print 'Read script with Baidu OCR ...'
resp = urllib2.urlopen(req)
print type(resp)
#print resp_jason
content = resp.read()
print content
print 'type of content is %s' % type(content)

#---- get OCR words ----
resp_json=json.loads(content)
num=int(resp_json['words_result_num'])
print '--- OCR get %d words as: '% num,
for i in range(0,num):
  print ' %s' % resp_json['words_result'][i]['words'],
print '---'

print 'parsing command ...'


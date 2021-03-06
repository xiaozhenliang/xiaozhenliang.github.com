---
layout: post
title: "Python实现Object-XML转换"
description: "Python实现Object-XML转换"
category: scripts
tags: ["Python"]
---

<pre>
def getXML(obj, objname=None):
    """getXML(obj, objname=None)
    returns an object as XML where Python object names are the tags.
    u={'UserID':10,'Name':'Yuki','Group':['Admin','Webmaster']}
    getXML(u,'User')
    '<User><UserID>10</UserID><Name>Yuki</Name><Group>Admin</Group><Group>Webmaster</Group></User>'
    """
    if obj == None:
        return ""
    if not objname:
        objname = "o2x"
    adapt={
        dict: getXML_dict,
        list: getXML_list,
        tuple: getXML_list,
        }
    if adapt.has_key(obj.__class__):
        return adapt[obj.__class__](obj, objname)
    else:
        return "<%(n)s>%(o)s</%(n)s>"%{'n':objname,'o':str(obj)}

def getXML_dict(indict, objname=None):
    h = "<%s>"%objname
    for k, v in indict.items():
        h += getXML(v, k)
    h += "</%s>"%objname
    return h

def getXML_list(inlist, objname=None):
    h = ""
    for i in inlist:
        h += getXML(i, objname)
    return h
</pre>

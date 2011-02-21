#!/usr/bin/env python

import sys,os,subprocess,socket,commands
from xml.dom.minidom import parse

from rtctree.tree import *
from rtctree.path import parse_path

def rtmpath():
    path_env=os.environ.get("RTM_PACKAGE_PATH")
    path_list=[]
    if path_env:
        path_list=path_env.split(":")
    path_list.append(".")
    return path_list


def rtmdepend(pack):
    fullpath=rtmpack(["find",pack])
    if not fullpath=="":
        depfile=fullpath + "/manifest.xml"
        if os.path.exists(depfile):
            depxml=parse(depfile)
            nodes=depxml.getElementsByTagName("package")[0].childNodes
            deps=[]
            for node in nodes:
                if node.nodeType==node.ELEMENT_NODE:
                    if node.tagName=="depend":
                        dep_tmp=node.attributes.get("package").value
                        deps.append(dep_tmp)
            return(deps)
    return([])

def find_elem(lst,elem):
    for it in lst:
        if it == elem:
            return elem
    return None

def rtmdepend_chain(pack):
    parents=[]
    children=[pack]

    while len(children)!=0:
        nchildren=[]
        for child in children:
            parents.append(child)
            nchildren.append(child)
        children=[]

        for child in nchildren:
            deps=rtmdepend(child)
            for dep in deps:
                if find_elem(parents,dep):
                    parents.remove(dep)
                    parents.append(dep)
                else:
                    children.append(dep)
    parents.remove(pack)
    parents.reverse()
    return(parents)

def rtmpack(args):
    path_list=rtmpath()
    
    if args[0]=="find":
        for path in path_list:
            for dir_name in os.listdir(path):
                if dir_name == args[1]:
                    return(path + "/" + args[1])
    elif args[0]=="depend":
        #return(rtmdepend(args[1]))
        return(rtmdepend_chain(args[1]))
    else:
        print >>sys.stderr, "rtmpack: unknown arg: " + args[0]

    return("")

def rtmcd(arg):
    path=rtmpack(["find",arg])
    if path == "":
        print >>sys.stderr, "rtmcd: package not found: " + arg
    elif not os.path.isdir(path):
        print >>sys.stderr, "rtmcd: not a directry: " + arg
    else:
        os.chdir(path)

def rtmmake(arg,clean=False,nochain=False):
    path=rtmpack(["find",arg])
    if path == "":
        print >>sys.stderr, "rtmmake: package not found: " + arg
    elif not os.path.isdir(path):
        print >>sys.stderr, "rtmmake: not a directry: " + arg
    else:
        if not nochain:
            deps=rtmdepend_chain(arg)
            for dep in deps:
                rtmmake(dep,clean,True)
        rtmcd(arg)
        if os.path.exists("Makefile"):
            if clean:
                os.system("make clean")
            else:
                os.system("make")
        else:
            print >> sys.stderr, "rtmmake: no Makefile found: " + arg

def rtmrun(pack,comp,delay):
    path=rtmpack(["find",pack])
    pythonpath=commands.getoutput("which python")
    if os.environ.has_key("EUS"):
        if os.environ["EUS"] == "jskrbeusgl":
            euslisppath=commands.getoutput("which jskrbeusgl")
    else:
        euslisppath=commands.getoutput("which irteusgl")
    tmppaths=commands.getoutput("which -a invoke_command_with_sleep.sh").split('\n')
    invoke_commandpath=tmppaths[len(tmppaths)-1]
        
    if path != "":
        if comp.find(".py") >= 0:
            return subprocess.Popen(pythonpath + " " + path + "/" + comp, cwd=path, shell=True)
        elif comp.find(".l") >= 0:
            return subprocess.Popen(invoke_commandpath + " " + delay + " " + euslisppath + " " + path + "/" + comp, cwd=path, shell=True)
        else:
            return subprocess.Popen(path + "/" + comp, cwd=path)
    else:
        return None

def rtmrun_with_tabs(packs,comps,delays):
    pythonpath=commands.getoutput("which python")
    if os.environ.has_key("EUS"):
        if os.environ["EUS"] == "jskrbeusgl":
            euslisppath=commands.getoutput("which jskrbeusgl")
    else:
        euslisppath=commands.getoutput("which irteusgl")
    tmppaths=commands.getoutput("which -a invoke_command_with_sleep.sh").split('\n')
    invoke_commandpath=tmppaths[len(tmppaths)-1]

    tabcommands=[]

    for (pack, comp, delay) in zip(packs, comps, delays):
        path=rtmpack(["find",pack])
        if path != "":
            if comp.find(".py") >= 0:
                tabcommands.append("--tab -t " + comp + " -e \"" + pythonpath + " " + path + "/" + comp + "\"" + " --working-directory=" + path)
            elif comp.find(".l") >= 0:
                tabcommands.append("--tab -t " + comp + " -e \"" + invoke_commandpath + " " + delay + " " + euslisppath + " " + path + "/" + comp + "\"" + " --working-directory=" + path)
            else:
                tabcommands.append("--tab -t " + comp + " -e \"" + path + "/" + comp + "\"" + " --working-directory=" + path)

    return subprocess.Popen("/usr/bin/gnome-terminal " + " ".join(tabcommands), shell=True)

# for rtmlaunch
class rtmprocess:
    def __init__(self):
        self.package=""
        self.comp=""
        self.delay=""

    def context_name(self):
        return self.comp

    def debug_print(self):
        print "  process:"
        print "   package: " + self.package
        print "   comp: " + self.comp
        print "   delay: " + self.delay

class rtmcomponent:
    def __init__(self):
        self.package=""
        self.comp=""
        self.context=""
        self.formats=""
        self.execute=True
        self.dstate="activate"
        self.ainterval=0

    def context_name(self):
        return self.formats.replace("%h",socket.gethostname()).replace("%n",self.context)

    def debug_print(self):
        print "  rtmcomponent:"
        print "   package: " + self.package
        print "   comp: " + self.comp
        print "   context: " + self.context
        print "   formats: " + self.formats
        print "   context_name: " + self.context_name()
        print "   execute: " + str(self.execute)

class rtmport:
    def __init__(self):
        self.context=""
        self.portname=""

    def debug_print(self):
        print "  rtmport:"
        print "   context: " + self.context
        print "   portname: " + self.portname

class rtmconnector:
    def __init__(self):
        self.outport=None
        self.inports=[];
        self.services=[];

    def debug_print(self):
        print " rtmconnector:"
        print " outport: "
        if self.outport:
            self.outport.debug_print()
        else:
            print "  None"
        print " inports: "
        for inport in self.inports:
            inport.debug_print()
        print " services: "
        for sport in self.services:
            sport.debug_print()


class rtmlauncher:
    def __init__(self):
        self.nameserver="localhost:9876"
        self.components=[]
        self.processes=[]
        self.configurations=[]
        self.connectors=[]
        self.invoketype=None

    def set_component(self,pack,comp,cxt,exe,dstate,ainterval):
        rtmcomp=rtmcomponent()
        rtmcomp.package=pack
        rtmcomp.comp=comp
        rtmcomp.context=cxt
        rtmcomp.execute=exe
        rtmcomp.dstate=dstate
        rtmcomp.ainterval=ainterval
        rtcconfattr=read_rtc_conf(pack)
        if rtcconfattr:
            rtmcomp.formats=rtcconfattr.get("naming.formats")
        self.components.append(rtmcomp)

    def set_process(self,pack,proc,delay):
        rtmproc=rtmprocess()
        rtmproc.package=pack
        rtmproc.comp=proc
        rtmproc.delay=delay
        self.processes.append(rtmproc)

    def debug_print(self):
        print "rtmlauncer:"
        print "nameserver: " + self.nameserver
        print "components:"
        for comp in self.components:
            comp.debug_print()
        print "connectors:"
        for conn in self.connectors:
            conn.debug_print()

def read_rtc_conf(pack):
    fullpath=rtmpack(["find",pack])
    if not fullpath=="":
        conffile=fullpath + "/rtc.conf"
        if os.path.exists(conffile):
            f=open(conffile)
            lines=f.readlines()
            f.close()

            params={}

            for line in lines:
                lline=line.lstrip().rstrip()
                if lline != "" and lline[0] != "#":
                    (com,col,opr)=lline.partition(":")
                    params[com]=opr.lstrip()
            return params
    return None

def read_launch_xml(xmlfile):
    if os.path.exists(xmlfile):
        rtml=rtmlauncher()
        xmldom=parse(xmlfile)
        nodes=xmldom.getElementsByTagName("xml")[0].childNodes
        for node in nodes:
            if node.nodeType==node.ELEMENT_NODE:
                if node.tagName=="nameserver":
                    ns_host=node.attributes.get("host").value
                    #ns_port=node.attributes.get("port").value
                    ns_port_attr=node.attributes.get("port")
                    if ns_port_attr:
                        rtml.nameserver=ns_host + ":" + ns_port_attr.value
                    else:
                        rtml.nameserver=ns_host
                if node.tagName=="terminal-invoke":
                    if node.attributes.get("type")!=None:
                        rtml.invoketype=node.attributes.get("type").value
                if node.tagName=="component":
                    rtc_pack=node.attributes.get("package").value
                    rtc_comp=node.attributes.get("comp").value
                    rtc_cxt=node.attributes.get("context").value
                    if node.attributes.get("defaultstate")!=None:
                        rtc_dstate=node.attributes.get("defaultstate").value
                    else:
                        rtc_dstate=None
                    if node.attributes.get("activation_interval")!=None:
                        rtc_ainterval=float(node.attributes.get("activation_interval").value)
                    else:
                        rtc_ainterval=None

                    rtc_exe=True
                    rtc_exe_attr=node.attributes.get("execute")
                    if rtc_exe_attr:
                        rtc_exe=(rtc_exe_attr.value=="true")

                    rtml.set_component(rtc_pack,rtc_comp,rtc_cxt,rtc_exe, rtc_dstate, rtc_ainterval)
                    for conf in node.childNodes:
                        if conf.nodeType==node.ELEMENT_NODE:
                            if conf.tagName=="configuration":
                                cparams = [rtc_cxt, conf.attributes.get("name").value, conf.attributes.get("value").value]
                                rtml.configurations.append(cparams)

                if node.tagName=="process":
                    rtc_pack=node.attributes.get("package").value
                    rtc_proc=node.attributes.get("proc").value
                    if node.attributes.get("delaytime")!=None:
                        rtc_delay=node.attributes.get("delaytime").value
                    else:
                        rtc_delay="0"
                    rtml.set_process(rtc_pack,rtc_proc, rtc_delay)

                elif node.tagName=="connection":
                    rtmcon=rtmconnector()
                    for port in node.childNodes:
                        if port.nodeType==node.ELEMENT_NODE:
                            if port.tagName=="port":
                                rtmp=rtmport()
                                port_type=port.attributes.get("type").value
                                rtmp.portname=port.attributes.get("name").value
                                rtmp.context=port.attributes.get("context").value
                                if port_type=="in":
                                    rtmcon.inports.append(rtmp)
                                elif port_type=="out":
                                    rtmcon.outport=rtmp
                                elif port_type=="service":
                                    rtmcon.services.append(rtmp)
                    rtml.connectors.append(rtmcon)
        return rtml
    return None

def alive_component(cxtname,ns=["localhost"]):
    rtree=RTCTree(ns)
    cxtpath,ret=parse_path(cxtname)
    return rtree.has_path(cxtpath)

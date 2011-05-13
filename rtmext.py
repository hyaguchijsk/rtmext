#!/usr/bin/env python

import sys,os,subprocess,socket,commands,time
from xml.dom.minidom import parse

from rtctree.tree import *
from rtctree.path import parse_path
from rtshell import rtls,rtcon,rtact,rtdeact,rtconf,rtfind,rtmgr

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

def search_package(pkname, path_list):
    for path in path_list:
        for dir_name in os.listdir(path):
            if os.path.isdir(path + "/" + dir_name):
                if dir_name == pkname:
                    return(path + "/" + pkname)
                else:
                    if path != '.':
                        result = search_package(pkname, [path + "/" + dir_name])
                        if result!=None:
                            return result
    return None

def search_file(compname, searchpath):
    for cname in os.listdir(searchpath):
        if os.path.isdir(searchpath + "/" + cname):
            result = search_file(compname, searchpath + "/" + cname)
            if result!=None:
                return result
        else:
            if cname == compname:
                return(searchpath + "/" + compname)
    return None

def rtmpack(args):  
    if args[0]=="find":
        path_list=rtmpath()
        pkpath = search_package(args[1], path_list)
        if pkpath != None:
            return pkpath
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

def ordsum(str):
    sum = 0
    for chr in str:
        sum += ord(chr)
    return sum

def rtmrun(pack, comp, cxt, delay, rate=None, host=None, port=None):
    path=rtmpack(["find",pack])
    sproc=None
    pythonpath=commands.getoutput("which python")
    if os.environ.has_key("EUS"):
        if os.environ["EUS"] == "jskrbeusgl":
            euslisppath=commands.getoutput("which jskrbeusgl")
    else:
        euslisppath=commands.getoutput("which irteusgl")
    tmppaths=commands.getoutput("which -a invoke_command_with_sleep.sh").split('\n')
    invoke_commandpath=tmppaths[len(tmppaths)-1]

    if host == None:
        host = socket.gethostname()
    if port == None:
        port = 2809

    rtcdcommand = None
    baseno = 2809

    if comp.find('.so')!=-1:
        cpos=comp.find(':')
        if cpos != -1:
            exec_context = comp[cpos+1:]
            comp = comp[:cpos]
        else:
            exec_context = None
        #use rtmgr for creating components    
        uname = '_' + str(cxt)

        tmpfn = "/tmp/rtcconf" + uname + ".XXX"
        tmpconf = open(tmpfn, 'w')
        tmpconf.write("corba.nameservers: " + host + ":" + str(port) + "\n")
        tmpconf.write("logger.enable: NO\n")
        tmpconf.write("manager.instance_name: rtmext_manager" + uname + "\n")
        mmport = ordsum(uname) + baseno
        tmpconf.write("corba.master_manager:" + host + ":" + str(mmport) + "\n")
        tmpconf.write("manager.modules.load_path: ./\n")
        tmpconf.write("manager.modules.abs_path_allowed: yes\n")
        if rate == None:
            rate = 1000
        tmpconf.write("exec_cxt.periodic.rate: "+ str(rate) + "\n")
        if exec_context!=None:
            tmpconf.write("exec_cxt.periodic.type: " + str(exec_context) + "\n")
        tmpconf.close()
        rtcdcommand = "rtcd rtmextmgr -f " + tmpfn + " -d"

        sproc = subprocess.Popen(rtcdcommand, cwd=path, shell=True)

        time.sleep(0.1)
        #loading & creating components
        rtfret , manpath = rtfind.main(['/' + host, '--name=rtmext_manager' + uname + '.mgr', '--type=m'], None)
        if exec_context !=None:
            ec_sofile = exec_context + '.so'
            ec_initfunc = exec_context + 'Init'
            ecpath = search_file( ec_sofile, path)
            rtmgr.main(['--load=' + str(ecpath), '--init-func=' + str(ec_initfunc), manpath[0]], None)
        sopath = search_file(comp, path)
        classname = comp[:len(comp)-3]
        initfuncname = classname + "Init"
        rtmgr.main(['--load=' + str(sopath) , '--init-func=' + str(initfuncname), manpath[0]], None)
        rtmgr.main(['--create=' + str(classname) + '?instance_name=' + str(cxt), manpath[0]], None)
        time.sleep(0.5)

        #removing temporary files
        os.remove(tmpfn)
    else:
        comppath = search_file(comp, path)

        if path != "":
            if comp.find(".py") >= 0:
                sproc = subprocess.Popen(pythonpath + " " + comppath, cwd=path, shell=True)
            elif comp.find(".l") >= 0:
                sproc = subprocess.Popen(invoke_commandpath + " " + delay + " " + euslisppath + " " + comppath, cwd=path, shell=True)
            else:
                sproc = subprocess.Popen(comppath, cwd=path)
        else:
            sproc = None

    return [sproc, rtcdcommand]

def rtmrun_with_tabs(packs, comps, cxts, delays, rates=None, host=None, port=None):
    pythonpath=commands.getoutput("which python")
    if os.environ.has_key("EUS"):
        if os.environ["EUS"] == "jskrbeusgl":
            euslisppath=commands.getoutput("which jskrbeusgl")
    else:
        euslisppath=commands.getoutput("which irteusgl")
    tmppaths=commands.getoutput("which -a invoke_command_with_sleep.sh").split('\n')
    invoke_commandpath=tmppaths[len(tmppaths)-1]

    tabcommands=[]
    rtcdcommands = []
    unames = []
    baseno = 2809

    if host == None:
        host = socket.gethostname()
    if port == None:
        port = 2809

    for (pack, comp, cxt, rte, delay) in zip(packs, comps, cxts, rates, delays):
        path=rtmpack(["find",pack])
        rtcdcommand = None
        if comp.find('.so')!=-1:
            cpos=comp.find(':')
            if cpos != -1:
                exec_context = comp[cpos+1:]
                comp = comp[:cpos]
            else:
                exec_context = None

            #use rtmgr for creating components
            uname = '_' + str(cxt)

            tmpfn = "/tmp/rtcconf" + uname + ".XXX"
            tmpconf = open(tmpfn, 'w')
            tmpconf.write("corba.nameservers: " + host + ":" + str(port) + "\n")
            tmpconf.write("logger.enable: NO\n")
            tmpconf.write("manager.instance_name: rtmext_manager" + uname + "\n")
            mmport = ordsum(uname) + baseno
            tmpconf.write("corba.master_manager:" + host + ":" + str(mmport) + "\n")
            tmpconf.write("manager.modules.load_path: ./\n")
            tmpconf.write("manager.modules.abs_path_allowed: yes\n")
            if rte == None:
                rte = 1000
            tmpconf.write("exec_cxt.periodic.rate: " + str(rte) + "\n")
            if exec_context!=None:
                tmpconf.write("exec_cxt.periodic.type: " + str(exec_context) + "\n")                
            tmpconf.close()
            rtcdcommand = "rtcd rtmextmgr -f " + tmpfn + " -d"
        
            tabcommands.append("--tab -t " + cxt + " -e \"" + rtcdcommand + "\"" + " --working-directory=" + path)
            unames.append(uname)
        else:
            comppath = search_file(comp, path)
            if path != "" and comppath != None:
                if comp.find(".py") >= 0:
                    tabcommands.append("--tab -t " + comp + " -e \"" + pythonpath + " " + comppath + "\"" + " --working-directory=" + path)
                elif comp.find(".l") >= 0:
                    tabcommands.append("--tab -t " + comp + " -e \"" + invoke_commandpath + " " + delay + " " + euslisppath + " " + comppath + "\"" + " --working-directory=" + path)
                else:
                    tabcommands.append("--tab -t " + comp + " -e \"" + comppath + "\"" + " --working-directory=" + path)

        rtcdcommands.append(rtcdcommand)

    if tabcommands != []:
        sprocs = subprocess.Popen("/usr/bin/gnome-terminal " + " ".join(tabcommands), shell=True)

    time.sleep(0.1)
    #loading & creating components from manager
    manager_no = 0
    for (pack, comp, cxt) in zip(packs, comps, cxts):
        if comp.find('.so')!=-1:
            cpos=comp.find(':')
            if cpos != -1:
                exec_context = comp[cpos+1:]
                comp = comp[:cpos]
            else:
                exec_context = None

            rtfret , manpath = rtfind.main(['/' + host, '--name=rtmext_manager' + unames[manager_no] + '.mgr', '--type=m'], None)
            path=rtmpack(["find",pack])
            sopath = search_file(comp, path)
            classname = comp[:len(comp)-3]
            initfuncname = classname + "Init"
            if manpath !=[]:
                if exec_context !=None:
                    ec_sofile = exec_context + '.so'
                    ec_initfunc = exec_context + 'Init'
                    ecpath = search_file( ec_sofile, path)
                    rtmgr.main(['--load=' + str(ecpath), '--init-func=' + str(ec_initfunc), manpath[0]], None)
                rtmgr.main(['--load=' + str(sopath) , '--init-func=' + str(initfuncname), manpath[0]], None)
                rtmgr.main(['--create=' + str(classname) + '?instance_name=' + str(cxt), manpath[0]], None)
                time.sleep(0.5)
                manager_no += 1
        
    #removing temporary files
    for i in range(manager_no):
        tmpfn = "/tmp/rtcconf" + unames[i] + ".XXX"
        os.remove(tmpfn)

    return [sprocs, rtcdcommands]

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
        self.execrate=None

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

    def set_component(self,pack,comp,cxt,exe,dstate,ainterval,execrate):
        rtmcomp=rtmcomponent()
        rtmcomp.package=pack
        rtmcomp.comp=comp
        rtmcomp.context=cxt
        rtmcomp.execute=exe
        rtmcomp.dstate=dstate
        rtmcomp.ainterval=ainterval
        rtmcomp.execrate=execrate
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
        conffile=search_file('rtc.conf', fullpath)
        if conffile and os.path.exists(conffile):
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

def read_launch_xml(xmlfile, launchpath=None):
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

                    if node.attributes.get("execrate")!=None:
                        rtc_execrate=float(node.attributes.get("execrate").value)
                    else:
                        rtc_execrate=None

                    rtc_exe=True
                    rtc_exe_attr=node.attributes.get("execute")
                    if rtc_exe_attr:
                        rtc_exe=(rtc_exe_attr.value=="true")
                    rtml.set_component(rtc_pack,rtc_comp,rtc_cxt,rtc_exe, rtc_dstate, rtc_ainterval, rtc_execrate)
                    for conf in node.childNodes:
                        if conf.nodeType==node.ELEMENT_NODE:
                            if conf.tagName=="configuration":
                                vvalue =conf.attributes.get("value").value 
                                if conf.attributes.get("filesearch") != None:
                                    if conf.attributes.get("filesearch").value == 'yes':
                                        packpath=rtmpack(["find",rtc_pack])
                                        vvalue=search_file(conf.attributes.get("value").value, launchpath)
                                cparams = [rtc_cxt, conf.attributes.get("name").value, vvalue]
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

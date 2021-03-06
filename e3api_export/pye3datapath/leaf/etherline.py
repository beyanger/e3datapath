#! /usr/bin/python3
import tabulate
from ctypes import *
from pye3datapath.e3client import clib
from pye3datapath.e3client import api_call_exception
from pye3datapath.e3client import api_return_exception
from pye3datapath.e3client import register_service_endpoint

MAX_E_LINE_SERVICES=4096

class ether_eline(Structure):
    _pack_=1
    _fields_=[('is_cbp_ready',c_uint8,1),
                ('is_csp_ready',c_uint8,1),
                ('is_valid',c_uint8,1),
                ('reserved0',c_uint8),
                ('index',c_int16),
                ('label_to_push',c_uint32),
                ('NHLFE',c_int16),
                ('e3iface',c_int16),
                ('vlan_tci',c_uint16),
                ('ref_cnt',c_int16)]
    def __init__(self):
        pass
    def clone(self):
        e=ether_eline()
        e.is_cbp_ready  =self.is_cbp_ready
        e.is_csp_ready  =self.is_csp_ready
        e.is_valid      =self.is_valid
        e.reserved0     =self.reserved0
        e.index         =self.index
        e.label_to_push =self.label_to_push
        e.NHLFE         =self.NHLFE
        e.e3iface       =self.e3iface
        e.vlan_tci      =self.vlan_tci
        e.ref_cnt       =self.ref_cnt
        return e

    def __str__(self):
        ret=dict()
        ret['is_cbp_ready']=self.is_cbp_ready
        ret['is_csp_ready']=self.is_csp_ready
        ret['is_valid']=self.is_valid
        ret['reserved0']=self.reserved0
        ret['index']=self.index
        ret['label_to_push']=self.label_to_push
        ret['NHLFE']=self.NHLFE
        ret['e3iface']=self.e3iface
        ret['vlan_tci']=self.vlan_tci
        ret['ref_cnt']=self.ref_cnt
        return str(ret)

    def dump_definition(self):
        print(ether_eline.is_cbp_ready,':ether_eline.is_cbp_ready')
        print(ether_eline.is_csp_ready,':ether_eline.is_csp_ready')
        print(ether_eline.is_valid,':ether_eline.is_valid')
        print(ether_eline.reserved0,':ether_eline.reserved0')
        print(ether_eline.index,':ether_eline.index')
        print(ether_eline.label_to_push,':ether_eline.label_to_push')
        print(ether_eline.NHLFE,':ether_eline.NHLFE')
        print(ether_eline.e3iface,':ether_eline.e3iface')
        print(ether_eline.vlan_tci,':ether_eline.vlan_tci')
        print(ether_eline.ref_cnt,':ether_eline.ref_cnt')

def register_ether_line_service():
    api_ret=c_uint64(0)
    eline_number=c_uint32(0)
    rc=clib.leaf_api_register_e_service(byref(api_ret),1,byref(eline_number))
    if rc!=0:
        raise api_call_exception()
    if api_ret.value!=0:
        raise api_return_exception('make sure eline resource is not running out,api_ret:%x'%(api_ret.value))
    return eline_number.value

def get_ether_line_service(index):
    api_ret=c_uint64(0)
    eline_number=c_uint32(index)
    eline=ether_eline()
    rc=clib.leaf_api_get_e_line(byref(api_ret),eline_number,byref(eline))
    if rc!=0:
        raise api_call_exception()
    if api_ret.value!=0:
        raise api_return_exception('invalid e-line index or something else is wrong,api_ret:%x'%(api_ret.value))
    return eline
def list_ether_line_services():
    lst=list()
    api_ret=c_int64(0)
    nr_entries=c_int32(0)
    class A(Structure):
        _pack_=1
        _fields_=[('a',c_int16*MAX_E_LINE_SERVICES)]
    a=A()
    rc=clib.leaf_api_list_e_services(byref(api_ret),1,byref(nr_entries),byref(a))
    if rc!=0:
        raise api_call_exception()
    if api_ret.value!=0:
        raise api_return_exception('never suppose it happens here,api_ret:%x'%(api_ret.value))
    for i in range(nr_entries.value):
        lst.append(a.a[i])
    return lst
def delete_ether_line_service(index):
    api_ret=c_int64(0)
    service_index=c_int32(index)
    rc=clib.leaf_api_delete_e_service(byref(api_ret),1,service_index)
    if rc!=0:
        raise api_call_exception()
    if api_ret.value!=0:
        raise api_return_exception('make sure it\'s deletable first,api_ret:%x'%(api_ret.value))
def register_ether_line_port(eline_index,e3iface,vlan_tci):
    api_ret=c_int64(0)
    _eline_index=c_int16(eline_index)
    _e3iface=c_int32(e3iface)
    _vlan_tci=c_int32(vlan_tci)
    rc=clib.leaf_api_register_e_service_port(byref(api_ret),1,_eline_index,_e3iface,_vlan_tci)
    if rc!=0:
        raise api_call_exception()
    if api_ret.value!=0:
        raise api_return_exception('it conflicts with other e-line services or parameter is not valid,api_ret:%x'%(api_ret.value))
def delete_ether_line_port(eline_index):
    api_ret=c_int64(0)
    _eline_index=c_int16(eline_index)
    rc=clib.leaf_api_delete_e_service_port(byref(api_ret),1,_eline_index,0)
    if rc!=0:
        raise api_call_exception()
    if api_ret.value!=0:
        raise api_return_exception('make sure this eline port is deletable,api_ret:%x'%(api_ret.value))
def register_ether_line_nhlfe(eline_index,nhlfe,label_to_push):
    api_ret=c_int64(0)
    _eline_index=c_int16(eline_index)
    _nhlfe=c_int32(nhlfe)
    _label_to_push=c_int32(label_to_push)
    rc=clib.leaf_api_register_e_service_nhlfe(byref(api_ret),1,_eline_index,_nhlfe,_label_to_push)
    if rc!=0:
        raise api_call_exception()
    if api_ret.value!=0:
        raise api_return_exception('something is wrong with the arguments,api_ret:%x'%(api_ret.value))
def delete_ether_line_nhlfe(eline_index):
    api_ret=c_int64(0)
    _eline_index=c_int16(eline_index)
    rc=clib.leaf_api_delete_e_service_nhlfe(byref(api_ret),1,_eline_index)
    if rc!=0:
        raise api_call_exception()
    if api_ret.value!=0:
        raise api_return_exception('deleting eline nhlfe fails,api_ret:%x'%(api_ret.value))
def tabulate_ether_line_services():
    eline_lst=list_ether_line_services()
    table=list()
    for eline_idx in eline_lst:
        eline=get_ether_line_service(eline_idx)
        csp_info=''
        if eline.is_csp_ready!=0:
            csp_info='{iface:%d,vlan:%d}'%(eline.e3iface,eline.vlan_tci)
        cbp_info=''
        if eline.is_cbp_ready!=0:
            cbp_info='{nhlfe:%d,label:%d}'%(eline.NHLFE,eline.label_to_push)
        table.append([eline.index,
                eline.ref_cnt,
                csp_info,
                cbp_info])
    print(tabulate.tabulate(table,['index','ref count','csp info','cbp info'],tablefmt='psql'))

if __name__=='__main__':
    from pye3datapath.common.neighbor import *
    from pye3datapath.common.nexthop import *
    register_service_endpoint('ipc:///var/run/e3datapath.sock')
    register_neighbor('130.140.150.1','08:00:27:ab:24:62')
    register_nexthop(0,0)
    #print(sizeof(ether_eline))
    #ether_eline().dump_definition()
    print(register_ether_line_service())
    #print(get_ether_line_service(0))
    #print(get_ether_line_service(1))
    print(register_ether_line_port(0,1,4095))
    tabulate_ether_line_services()
    print(register_ether_line_nhlfe(0,0,0x123))
    tabulate_ether_line_services()
    #print(delete_ether_line_port(0))
    #print(delete_ether_line_nhlfe(0))
    lst=list_ether_line_services()
    print(lst)
    for eline in lst:
        print(get_ether_line_service(eline))

    delete_ether_line_service(0)

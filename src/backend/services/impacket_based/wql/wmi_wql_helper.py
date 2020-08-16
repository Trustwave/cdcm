
from __future__ import division
from __future__ import print_function
import sys
from inspect import getmembers
from types import FunctionType
sys.path.append('./impacket')
import time
import logging
from pprint import pprint
from impacket.smbconnection import SMBConnection, SMB_DIALECT, SMB2_DIALECT_002, SMB2_DIALECT_21
from impacket.dcerpc.v5.dcomrt import DCOMConnection
from impacket.dcerpc.v5.dcom import wmi
from impacket.dcerpc.v5.dtypes import NULL

def attributes(obj):
    disallowed_names = {
        name for name, value in getmembers(type(obj))
        if isinstance(value, FunctionType)}
    return {
        name: getattr(obj, name) for name in dir(obj)
        if name[0] != '_' and name not in disallowed_names and hasattr(obj, name)}

def print_attributes(obj):
    pprint(attributes(obj))

OUTPUT_FILENAME = '__' + str(time.time())
CODEC = sys.stdout.encoding

class WMIQUERY:
    def __init__(self, addr=None, username='', password='', domain='', hashes=None, aesKey=None, doKerberos=False, kdcHost=None):
        self.__username = username
        self.__password = password
        self.__domain = domain
        self.__lmhash = ''
        self.__nthash = ''
        self.__aesKey = aesKey
        self.__doKerberos = doKerberos
        self.__kdcHost = kdcHost
        self.__addr = addr
        self.__dcom = None
        self.__iWbemServices = None
        if hashes is not None:
            self.__lmhash, self.__nthash = hashes.split(':')

    def connect(self,namespace='//./root/cimv2'):
        self.__dcom = DCOMConnection(self.__addr, self.__username, self.__password, self.__domain, self.__lmhash, self.__nthash,
                                     self.__aesKey, oxidResolver=True, doKerberos=self.__doKerberos, kdcHost=self.__kdcHost)
        try:
            iInterface = self.__dcom.CoCreateInstanceEx(wmi.CLSID_WbemLevel1Login, wmi.IID_IWbemLevel1Login)
            iWbemLevel1Login = wmi.IWbemLevel1Login(iInterface)
            self.__iWbemServices= iWbemLevel1Login.NTLMLogin(namespace, NULL, NULL)
            # if options.rpc_auth_level == 'privacy':
            #     iWbemServices.get_dce_rpc().set_auth_level(RPC_C_AUTHN_LEVEL_PKT_PRIVACY)
            # elif options.rpc_auth_level == 'integrity':
            #     iWbemServices.get_dce_rpc().set_auth_level(RPC_C_AUTHN_LEVEL_PKT_INTEGRITY)

            iWbemLevel1Login.RemRelease()

        except(Exception, KeyboardInterrupt) as e:
            if logging.getLogger().level == logging.DEBUG:
                import traceback
                traceback.print_exc()
                logging.error(str(e))
            self.close()

    def printReply(self, iEnum):
        printHeader = True
        ret = ''
        while True:
            try:
                pEnum = iEnum.Next(0xffffffff,1)[0]
                record = pEnum.getProperties()
                if printHeader is True:
                    ret += '| '
                    for col in record:
                        ret += col +' | '
                    ret += '\n'
                printHeader = False
                ret += '| '
                for key in record:
                    if type(record[key]['value']) is list:
                        for item in record[key]['value']:
                            ret += str(item) + ' '
                        ret += ' | '
                    else:
                        ret += str(record[key]['value'])+' | '
                ret += '\n'
            except Exception as e:
                if logging.getLogger().level == logging.DEBUG:
                    import traceback
                    traceback.print_exc()
                if str(e).find('S_FALSE') < 0:
                    raise
                else:
                    break
        iEnum.RemRelease()
        return ret

    def do_query(self,query):
        iEnumWbemClassObject = self.__iWbemServices.ExecQuery(query.strip('\n'))
        ret = self.printReply(iEnumWbemClassObject)
        iEnumWbemClassObject.RemRelease()
        return ret

    def close(self):
        if self.__dcom is not None:
            self.__dcom.disconnect()

if __name__ == '__main__':
    try:
        CODEC = 'utf-8'
        executer = WMIQUERY("%{host}", "%{username}", "%{password}")
        executer.connect()
        executer.do_query("select hotfixid, description, installedby, installdate from win32_quickfixengineering")
        print ("3")
        # time.sleep(600)
        executer.close()
        print ("4")
    except KeyboardInterrupt as e:
        logging.error(str(e))
    except Exception as e:
        import traceback
        traceback.print_exc()
        logging.error(str(e))
        sys.exit(1)
    sys.exit(0)


#CheckAccess

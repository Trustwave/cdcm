
from __future__ import division
from __future__ import print_function
import sys
from pprint import pprint
from inspect import getmembers
from types import FunctionType
sys.path.append('./impacket')
# import os
# import cmd
# import argparse
import time
import logging
# import ntpath
from pprint import pprint
# from impacket.examples import logger
# from impacket import version
from impacket.smbconnection import SMBConnection, SMB_DIALECT, SMB2_DIALECT_002, SMB2_DIALECT_21
from impacket.dcerpc.v5.dcomrt import DCOMConnection
from impacket.dcerpc.v5.dcom import wmi
from impacket.dcerpc.v5.dtypes import NULL
# from impacket.krb5.keytab import Keytab
# from six import PY2

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

class WMI_REG_EXEC_METHOD:
    def __init__(self, addr=None, username='', password='', domain='', namespace='//./root/cimv2', hashes=None, aesKey=None, doKerberos=False, kdcHost=None):
        self.__username = username
        self.__password = password
        self.__domain = domain
        self.__lmhash = ''
        self.__nthash = ''
        self.__aesKey = aesKey
        self.__doKerberos = doKerberos
        self.__kdcHost = kdcHost
        self.__addr = addr
        self.__conn = None
        self.__dcom = None
        self.__StdRegProv = None
        self.__namespace = namespace
        if hashes is not None:
            self.__lmhash, self.__nthash = hashes.split(':')

    def connect(self):
        self.__conn = SMBConnection(self.__addr, self.__addr)
        if self.__doKerberos is False:
            self.__conn.login(self.__username, self.__password, self.__domain, self.__lmhash, self.__nthash)
        else:
            self.__conn.kerberosLogin(self.__username, self.__password, self.__domain, self.__lmhash,
                                      self.__nthash, self.__aesKey, kdcHost=self.__kdcHost)

        dialect = self.__conn.getDialect()
        if dialect == SMB_DIALECT:
            logging.info("SMBv1 dialect used")
        elif dialect == SMB2_DIALECT_002:
            logging.info("SMBv2.0 dialect used")
        elif dialect == SMB2_DIALECT_21:
            logging.info("SMBv2.1 dialect used")
        else:
            logging.info("SMBv3.0 dialect used")
        self.__dcom = DCOMConnection(self.__addr, self.__username, self.__password, self.__domain, self.__lmhash, self.__nthash,
                                     self.__aesKey, oxidResolver=True, doKerberos=self.__doKerberos, kdcHost=self.__kdcHost)
        try:
            iInterface = self.__dcom.CoCreateInstanceEx(wmi.CLSID_WbemLevel1Login, wmi.IID_IWbemLevel1Login)
            iWbemLevel1Login = wmi.IWbemLevel1Login(iInterface)
            iWbemServices= iWbemLevel1Login.NTLMLogin('//./root/cimv2', NULL, NULL)
            iWbemLevel1Login.RemRelease()
            self.__StdRegProv, _ = iWbemServices.GetObject('StdRegProv')
            self.__StdRegProv.getMethods()
        except  (Exception, KeyboardInterrupt) as e:
            if logging.getLogger().level == logging.DEBUG:
                import traceback
                traceback.print_exc()
                logging.error(str(e))
            self.close()

    def EnumKey(self, key, hive=0x80000002):
        try:
            rr = self.__StdRegProv.EnumKey(hive, key)
            return rr
        except  (Exception, KeyboardInterrupt) as e:
            if logging.getLogger().level == logging.DEBUG:
                import traceback
                traceback.print_exc()
            logging.error(str(e))
            self.close()

    def EnumValues(self, key, hive=0x80000002):
        try:
            rr = self.__StdRegProv.EnumValues(hive, key)
            return rr
        except  (Exception, KeyboardInterrupt) as e:
            if logging.getLogger().level == logging.DEBUG:
                import traceback
                traceback.print_exc()
            logging.error(str(e))
            self.close()

    def GetBinaryValue(self, key, value, hive=0x80000002):
        try:
            rr = self.__StdRegProv.GetBinaryValue(hive, key,value)
            print(rr.uValue)
            return rr.uValue
        except  (Exception, KeyboardInterrupt) as e:
            if logging.getLogger().level == logging.DEBUG:
                import traceback
                traceback.print_exc()
            logging.error(str(e))
            self.close()

    def GetDWORDValue(self, key,value, hive=0x80000002):
        try:
            rr = self.__StdRegProv.GetDWORDValue(hive, key,value)
            return rr.uValue
        except  (Exception, KeyboardInterrupt) as e:
            if logging.getLogger().level == logging.DEBUG:
                import traceback
                traceback.print_exc()
            logging.error(str(e))
            self.close()

    def GetExpandedStringValue(self, key,value, hive=0x80000002):
        try:
            rr = self.__StdRegProv.GetExpandedStringValue(hive, key,value)
            return rr.sValue
        except  (Exception, KeyboardInterrupt) as e:
            if logging.getLogger().level == logging.DEBUG:
                import traceback
                traceback.print_exc()
            logging.error(str(e))
            self.close()

    def GetMultiStringValue(self, key,value, hive=0x80000002):
        try:
            rr = self.__StdRegProv.GetMultiStringValue(hive, key, value)
            ret = ""
            for x in rr.sValue:
                ret += x + "\n"
            return ret
        except  (Exception, KeyboardInterrupt) as e:
            if logging.getLogger().level == logging.DEBUG:
                import traceback
                traceback.print_exc()
            logging.error(str(e))
            self.close()

    def GetStringValue(self, key,value, hive=0x80000002):
        try:
            rr = self.__StdRegProv.GetStringValue(hive, key,value)
            return rr.sValue
        except  (Exception, KeyboardInterrupt) as e:
            if logging.getLogger().level == logging.DEBUG:
                import traceback
                traceback.print_exc()
            logging.error(str(e))
            self.close()

    def GetQWORDValue(self, key,value, hive=0x80000002):
        try:
            rr = self.__StdRegProv.GetQWORDValue(hive, key,value)
            return rr.uValue
        except  (Exception, KeyboardInterrupt) as e:
            if logging.getLogger().level == logging.DEBUG:
                import traceback
                traceback.print_exc()
            logging.error(str(e))
            self.close()

    # def GetSecurityDescriptor(self, key,value, hive=0x80000002):
    #     try:
    #         rr = self.__StdRegProv.GetSecurityDescriptor(hive, key)
    #         print ("AAA")
    #         print(rr)
    #         return rr
    #     except  (Exception, KeyboardInterrupt) as e:
    #         if logging.getLogger().level == logging.DEBUG:
    #             import traceback
    #             traceback.print_exc()
    #         logging.error(str(e))
    #         self.close()

    def close(self):
        if self.__conn is not None:
            self.__conn.logoff()
        if self.__dcom is not None:
            self.__dcom.disconnect()

# if __name__ == '__main__':
#     print("SSSSSS")
#     CODEC = 'utf-8'
#     executer = WMI_REG_EXEC_METHOD("%{host}", "%{username}", "%{password}")
  # return executer.enum("SOFTWARE")
    # except KeyboardInterrupt as e:
    #     logging.error(str(e))
    # except Exception as e:
    #     if logging.getLogger().level == logging.DEBUG:
    #         import traceback
    #         traceback.print_exc()
    #     logging.error(str(e))
    #     sys.exit(1)
    #
    # sys.exit(0)


#CheckAccess

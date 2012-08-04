using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.ComponentModel;

namespace WindSLIC_bootmgr
{
    public class Owner
    {
        private const int SE_FILE_OBJECT = 1;
        private const int OWNER_SECURITY_INFORMATION = 0x01;
        private const int NAME_SIZE = 64;
        private const int SID_SIZE = 32;

        public static void ChangeOwner(String s_Path, String s_UserName)
        {
            IntPtr pNewOwner, peUse;
            Win32Exception Win32Error;
            String domain_name;
            int ret, sid_len, domain_len;

            if (Privileges.SetPrivileges() == false)
                throw new Exception("Required privilege not held by the user");

            sid_len = SID_SIZE;
            pNewOwner = Marshal.AllocHGlobal(sid_len);
            domain_len = NAME_SIZE;
            domain_name = String.Empty.PadLeft(domain_len);
            peUse = IntPtr.Zero;

            if (!Imports.LookupAccountName(null, s_UserName, pNewOwner, ref sid_len, domain_name, ref domain_len, ref peUse))
            {
                ret = Marshal.GetLastWin32Error();
                Win32Error = new Win32Exception(ret);
                throw new Exception(Win32Error.Message);
            }
            ret = Imports.SetNamedSecurityInfo(s_Path, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION, pNewOwner, IntPtr.Zero, IntPtr.Zero, IntPtr.Zero);
            if (ret != 0)
            {
                Win32Error = new Win32Exception(ret);
                throw new Exception(Win32Error.Message);
            }
            Marshal.FreeHGlobal(pNewOwner);
        }
    }
}

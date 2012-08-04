using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Diagnostics;

namespace WindSLIC_bootmgr
{
    public class Privileges
    {
        const int TOKEN_QUERY = 0x08;
        const int TOKEN_ADJUST_PRIVILEGES = 0x20;
        const String SE_TAKE_OWNERSHIP_NAME = "SeTakeOwnershipPrivilege";
        const String SE_RESTORE_NAME = "SeRestorePrivilege";
        const int SE_PRIVILEGE_ENABLED = 0x02;

        public static bool SetPrivileges()
        {
            IntPtr hProc, hToken;
            long luid_TakeOwnership, luid_Restore;
            Imports.TOKEN_PRIVILEGES tp = new Imports.TOKEN_PRIVILEGES();

            hProc = Process.GetCurrentProcess().Handle;
            hToken = IntPtr.Zero;

            if (!Imports.OpenProcessToken(hProc, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, ref hToken))
                return false;

            luid_TakeOwnership = 0;

            if (!Imports.LookupPrivilegeValue(null, SE_TAKE_OWNERSHIP_NAME, ref luid_TakeOwnership))
                return false;

            luid_Restore = 0;

            if (!Imports.LookupPrivilegeValue(null, SE_RESTORE_NAME, ref luid_Restore))
                return false;

            tp.PrivilegeCount = 2;
            tp.Privilege1.Luid = luid_TakeOwnership;
            tp.Privilege1.Attributes = SE_PRIVILEGE_ENABLED;
            tp.Privilege2.Luid = luid_Restore;
            tp.Privilege2.Attributes = SE_PRIVILEGE_ENABLED;

            if (!Imports.AdjustTokenPrivileges(hToken, false, ref tp, 0, IntPtr.Zero, IntPtr.Zero))
                return false;

            return true;
        }
    }
}

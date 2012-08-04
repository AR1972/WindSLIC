using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.IO;
using Microsoft.Win32.SafeHandles;

namespace WindSLIC_bootmgr
{
    class Imports
    {
        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern FindFirstVolumeSafeHandle FindFirstVolume(
            StringBuilder lpszVolumeName,
            uint cchBufferLength);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern bool FindNextVolume(
            FindFirstVolumeSafeHandle hFindVolume,
            StringBuilder lpszVolumeName,
            uint cchBufferLength);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern bool FindVolumeClose(IntPtr hFindVolume);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern uint GetDriveType(string lprootPathName);

        [DllImport("kernel32.dll", CharSet = CharSet.Auto)]
        public static extern bool GetVolumeInformation(
            string Volume,
            StringBuilder VolumeName,
            uint VolumeNameSize,
            out uint SerialNumber,
            out uint SerialNumberLength,
            out uint flags,
            StringBuilder fs,
            uint fs_size);

        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool GetVolumePathNamesForVolumeNameW(
            [MarshalAs(UnmanagedType.LPWStr)]
            string lpszVolumeName,
            [MarshalAs(UnmanagedType.LPWStr)]
            string lpszVolumePathNames,
            uint cchBuferLength,
            ref UInt32 lpcchReturnLength);

        [DllImport("kernel32", SetLastError = true, CharSet = CharSet.Auto)]
        public static extern SafeFileHandle CreateFile(
            [MarshalAs(UnmanagedType.LPWStr)]
            String fileName,
            uint desiredAccess,
            FileShare shareMode,
            IntPtr securityAttrs,
            FileMode creationDisposition,
            int flagsAndAttributes,
            IntPtr templateFile);

        [DllImport("advapi32.dll")]
        public static extern int SetNamedSecurityInfo(
            String pObjectName,
            int ObjectType,
            int SecurityInfo,
            IntPtr psidOwner,
            IntPtr psidGroup,
            IntPtr pDacl,
            IntPtr pSacl);

        [DllImport("advapi32.dll")]
        public static extern bool LookupAccountName(
            String lpSystemName,
            String lpAccountName,
            IntPtr Sid,
            ref int cbSid,
            String lpReferencedDomainNam,
            ref int cchRefrencedDomainName,
            ref IntPtr peUse);

        [DllImport("advapi32.dll")]
        public static extern bool OpenProcessToken(
            IntPtr ProcessHandle,
            int DesiredAccess,
            ref IntPtr TokenHandle);

        [DllImport("advapi32.dll")]
        public static extern bool LookupPrivilegeValue(
            String lpSystemName,
            String lpName,
            ref long lpLuid);

        [DllImport("advapi32")]
        public static extern bool AdjustTokenPrivileges(
            IntPtr TokenHandle,
            bool DisableAllPrivileges,
            ref TOKEN_PRIVILEGES NewState,
            int bufferLength,
            IntPtr PreviousState,
            IntPtr ReturnLength);

        [DllImport("kernel32.dll", SetLastError = true, CharSet = CharSet.Auto)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool SetFileTime(
            SafeFileHandle hFile,
            ref long lpCreationTime,
            ref long lpLastAccessTime,
            ref long lpLastWriteTime);

        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        public struct TOKEN_PRIVILEGES
        {
            public int PrivilegeCount;
            public LUID_AND_ATTRIBUTES Privilege1;
            public LUID_AND_ATTRIBUTES Privilege2;
        }

        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        public struct LUID_AND_ATTRIBUTES
        {
            public long Luid;
            public int Attributes;
        }
    }
}

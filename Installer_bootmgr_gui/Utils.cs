using System;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;
using Microsoft.Win32.SafeHandles;
using System.Runtime.InteropServices;
using System.Security;
using System.Security.AccessControl;
using System.IO.Compression;
using System.IO;
using System.ComponentModel;

namespace WindSLIC_bootmgr
{
    class Utils
    {
        private const String TrustedInstaller = "NT SERVICE\\TrustedInstaller";
        private static String UserName = Environment.UserName;
        private static byte[] patchJMP = { 0xFD, 0x3E };
        private static byte[] unpatchJMP = { 0xD5, 0x01 };
        private static int patchOffset = 0x3F00;
        public static byte patchOEMID = 0x01;
        public static byte BIOSRW = 0x00;
        public static byte EBDAIgnored = 0x00;

        public enum EFileAccess : uint
        {
            GENERIC_READ = 0x80000000,
            GENERIC_WRITE = 0x40000000,
            GENERIC_EXECUTE = 0x20000000,
            GENERIC_ALL = 0x10000000
        };

        public struct VolumeInfo
        {
            public string VolumeGUID;
            public string VolumePath;
            public string VolumeName;
            public string FSType;
            public List<string> MountPoints;
            public DriveType Type;
            public bool bootmgr;
        }

        public enum DriveType : uint
        {
            DRIVE_UNKNOWN,
            DRIVE_NO_ROOT_DIR,
            DRIVE_REMOVABLE,
            DRIVE_FIXED,
            DRIVE_REMOTE,
            DRIVE_CDROM,
            DRIVE_RAMDISK
        };


        public static List<VolumeInfo> findBootmgr()
        {
            uint serialNum, serialNumLength, flags;
            StringBuilder volumename = new StringBuilder(256);
            StringBuilder fstype = new StringBuilder(256);
            List<string> volumes = GetVolumes();
            List<string> mountpoints;
            List<VolumeInfo> LstDI = new List<VolumeInfo>();
            foreach (String volume in volumes)
            {
                VolumeInfo di = new VolumeInfo();
                DriveType type = (DriveType)Imports.GetDriveType(volume);
                if (type == DriveType.DRIVE_FIXED)
                {
                    bool ok = Imports.GetVolumeInformation(volume, volumename, (uint)volumename.Capacity - 1,
                        out serialNum, out serialNumLength, out flags, fstype, (uint)fstype.Capacity - 1);
                    mountpoints = GetMountPointsForVolume(volume);
                    di.VolumeGUID = volume;
                    di.VolumePath = volume.Replace("\\\\?\\", "\\\\.\\");
                    di.VolumeName = volumename.ToString();
                    di.FSType = fstype.ToString();
                    di.MountPoints = mountpoints;
                    di.Type = type;
                    if (File.Exists(di.VolumePath + "bootmgr"))
                    {
                        di.bootmgr = true;
                        LstDI.Add(di);
                    }

                }
            }
            return LstDI;
        }

        public static byte[] ReadFile(string file, long length = 0)
        {
            try
            {
                SafeFileHandle fh = Imports.CreateFile(file, (int)FileAccess.Read, FileShare.ReadWrite, IntPtr.Zero, FileMode.OpenOrCreate, 0, IntPtr.Zero);
                if (fh.IsInvalid)
                {
                    return null;
                }
                FileStream fstream = new FileStream(fh, FileAccess.Read);
                if (length == 0)
                {
                    length = fstream.Length;
                }
                byte[] ret = new byte[length];
                fstream.Seek(0, SeekOrigin.Begin);
                fstream.Read(ret, 0, (int)length);
                fstream.Close();
                fstream.Dispose();
                fh.Close();
                fh.Dispose();
                return ret;
            }
            catch (Exception ex)
            {
                Debug.WriteLine("ReadFile()");
                Debug.WriteLine(ex.Message);
                return null;
            }
        }

        public static bool WriteFile(byte[] array, string file)
        {
            try
            {
                SafeFileHandle fh = Imports.CreateFile(file, (int)FileAccess.Write, FileShare.ReadWrite, IntPtr.Zero, FileMode.OpenOrCreate, 0, IntPtr.Zero);
                if (fh.IsInvalid)
                {
                    return false;
                }
                FileStream fstream = new FileStream(fh, FileAccess.Write);
                fstream.Seek(0, SeekOrigin.Begin);
                fstream.Write(array, 0, array.Length);
                fstream.Close();
                fstream.Dispose();
                fh.Close();
                fh.Dispose();
                return true;
            }
            catch (Exception ex)
            {
                Debug.WriteLine("WriteFile()");
                Debug.WriteLine(ex.Message);
                return false;
            }
        }

        public static List<string> GetVolumes()
        {
            try
            {
                const uint bufferLength = 1024;
                StringBuilder volume = new StringBuilder((int)bufferLength, (int)bufferLength);
                List<string> result = new List<string>();

                using (FindFirstVolumeSafeHandle volumeHandle = Imports.FindFirstVolume(volume, bufferLength))
                {
                    if (volumeHandle.IsInvalid)
                    {
                        throw new System.ComponentModel.Win32Exception(Marshal.GetLastWin32Error());
                    }

                    do
                    {
                        result.Add(volume.ToString());
                    } while (Imports.FindNextVolume(volumeHandle, volume, bufferLength));

                    return result;
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine("GetVolumes()");
                Debug.WriteLine(ex.Message);
                return new List<string>();
            }
        }


        public static List<string> GetMountPointsForVolume(string volumeDeviceName)
        {
            List<string> result = new List<string>();

            // GetVolumePathNamesForVolumeName is only available on Windows XP/2003 and above
            int osVersionMajor = Environment.OSVersion.Version.Major;
            int osVersionMinor = Environment.OSVersion.Version.Minor;
            if (osVersionMajor < 5 || (osVersionMajor == 5 && osVersionMinor < 1))
            {
                return result;
            }

            try
            {
                uint lpcchReturnLength = 0;
                string buffer = "";

                Imports.GetVolumePathNamesForVolumeNameW(volumeDeviceName, buffer, (uint)buffer.Length, ref lpcchReturnLength);
                if (lpcchReturnLength == 0)
                {
                    return result;
                }

                buffer = new string(new char[lpcchReturnLength]);

                if (!Imports.GetVolumePathNamesForVolumeNameW(volumeDeviceName, buffer, lpcchReturnLength, ref lpcchReturnLength))
                {
                    throw new Win32Exception(Marshal.GetLastWin32Error());
                }

                string[] mounts = buffer.Split('\0');
                foreach (string mount in mounts)
                {
                    if (mount.Length > 0)
                    {
                        result.Add(mount);
                    }
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine("GetMountPointsForVolume()");
                Debug.WriteLine(ex.Message);
            }

            return result;
        }

        public static void TakeOwnFile(String file)
        {
            try
            {
                if (File.Exists(file))
                {
                    WindSLIC_bootmgr.Owner.ChangeOwner(file, UserName);
                    FileSecurity fSecurity = File.GetAccessControl(file);
                    FileSystemAccessRule accessRule = new FileSystemAccessRule(
                        UserName, FileSystemRights.FullControl, AccessControlType.Allow);
                    fSecurity.AddAccessRule(accessRule);
                    File.SetAccessControl(file, fSecurity);
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine("TakeOwnFile");
                Debug.WriteLine(ex.Message);
            }
        }

        public static void GiveOwnFile(String file)
        {
            try
            {
                if (File.Exists(file))
                {
                    WindSLIC_bootmgr.Owner.ChangeOwner(file, TrustedInstaller);
                    FileSecurity fSecurity = File.GetAccessControl(file);
                    FileSystemAccessRule accessRule = new FileSystemAccessRule(
                        UserName, FileSystemRights.FullControl, AccessControlType.Allow);
                    fSecurity.RemoveAccessRule(accessRule);
                    File.SetAccessControl(file, fSecurity);
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine("GiveOwnFile");
                Debug.WriteLine(ex.Message);
            }
        }

        public static byte[] Decompress(byte[] file)
        {
            MemoryStream CompressedFile = new MemoryStream(file);
            byte[] sizebuffer = new byte[4];
            CompressedFile.Position = CompressedFile.Length - 4;
            CompressedFile.Read(sizebuffer, 0, 4);
            int filesize = BitConverter.ToInt32(sizebuffer, 0);
            byte[] DecompressedFile = new byte[filesize];
            CompressedFile.Position = 0;
            GZipStream gz = new GZipStream(CompressedFile, CompressionMode.Decompress);
            gz.Read(DecompressedFile, 0, filesize);
            gz.Close();
            gz.Dispose();
            CompressedFile.Dispose();
            return DecompressedFile;
        }

        public static void EncryptSLIC(ref byte[] SLIC, ref byte[] EncKey)
        {
            try
            {
                byte[] key = GenRndKey(EncKey.Length);
                int keyposition = 0;
                int encryptedbyte;
                for (int i = 0; i < SLIC.Length; i++)
                {
                    encryptedbyte = SLIC[i] + 0x7F + key[keyposition];
                    keyposition++;
                    if (encryptedbyte >= 256)
                    {
                        encryptedbyte -= 256;
                    }
                    SLIC[i] = (byte)encryptedbyte;
                    if (keyposition >= key.Length)
                    {
                        keyposition = 0;
                    }
                }
                for (int i = 0; i < EncKey.Length; i++)
                {
                    EncKey[i] = (byte)(key[i] ^ 0xFF);
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine("EncryptSLIC()");
                Debug.WriteLine(ex.Message);
            }
        }

        public static byte[] GenRndKey(int length)
        {
            byte[] rnd = new byte[length];
            Random r = new Random();
            r.NextBytes(rnd);
            return rnd;
        }

        public static void FindOffsets(byte[] patch, ref int SLICOffset, ref int KeyOffset)
        {
            try
            {
                for (int i = 0; i < patch.Length - 8; i++)
                {
                    if (patch[i] == 0x4B &&
                        patch[i + 1] == 0x45 &&
                        patch[i + 2] == 0x59 &&
                        patch[i + 3] == 0x4B &&
                        patch[i + 4] == 0x45 &&
                        patch[i + 5] == 0x59 &&
                        patch[i + 6] == 0x4B &&
                        patch[i + 7] == 0x45)
                    {
                        KeyOffset = i;
                    }
                    if (patch[i] == 0x53 &&
                        patch[i + 1] == 0x4C &&
                        patch[i + 2] == 0x49 &&
                        patch[i + 3] == 0x43 &&
                        patch[i + 4] == 0x76 &&
                        patch[i + 5] == 0x01 &&
                        patch[i + 6] == 0x44 &&
                        patch[i + 7] == 0x55)
                    {
                        SLICOffset = i;
                    }
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine("FindOffsets()");
                Debug.WriteLine(ex.Message);
            }
        }

        public static void SetFileTimes(string file, DateTime creationTime, DateTime accessTime, DateTime writeTime)
        {
            try
            {
                long lCreationTime = creationTime.ToFileTime();
                long lAccessTime = accessTime.ToFileTime();
                long lWriteTime = writeTime.ToFileTime();

                SafeFileHandle fh = Imports.CreateFile(file, (uint)EFileAccess.GENERIC_ALL,
                    FileShare.ReadWrite, IntPtr.Zero, FileMode.Open,
                    (int)FileAttributes.Normal, IntPtr.Zero);

                if (fh.IsInvalid)
                {
                    return;
                }

                if (!Imports.SetFileTime(fh, ref lCreationTime, ref lAccessTime, ref lWriteTime))
                {
                    fh.Close();
                    fh.Dispose();
                    throw new Win32Exception();
                }
                fh.Close();
                fh.Dispose();
            }
            catch (Exception ex)
            {
                Debug.WriteLine("SetFileTimes()");
                Debug.WriteLine(ex.Message);
            }
        }

        public static void PatchBootmgr(ref List<Utils.VolumeInfo> VolInfo, string SLIC)
        {
            foreach (Utils.VolumeInfo vi in VolInfo)
            {
                if (vi.VolumeName == "System Reserved" || vi.MountPoints.Contains("C:\\"))
                {
                    DateTime LastWrite = File.GetLastWriteTime(vi.VolumePath + "bootmgr");
                    DateTime LastAccess = File.GetLastAccessTime(vi.VolumePath + "bootmgr");
                    DateTime Created = File.GetCreationTime(vi.VolumePath + "bootmgr");
                    Utils.TakeOwnFile(vi.VolumePath + "bootmgr");
                    File.SetAttributes(vi.VolumePath + "bootmgr", FileAttributes.Normal);
                    byte[] file = Utils.ReadFile(vi.VolumePath + "bootmgr");
                    if (file != null && file.Length == 383786)
                    {
                        byte[] patch = Utils.Decompress(Properties.Resources.bootmgr);
                        int SLICOffset = 0;
                        int KeyOffset = 0;
                        Utils.FindOffsets(patch, ref SLICOffset, ref KeyOffset);
                        byte[] EncSLIC = File.ReadAllBytes(SLIC);
                        if (EncSLIC.Length != 374)
                        {
                            throw new System.IO.FileLoadException("SLIC lenght incorrect");
                        }
                        byte[] EncKey = new byte[8];
                        Utils.EncryptSLIC(ref EncSLIC, ref EncKey);
                        Array.ConstrainedCopy(EncKey, 0, patch, KeyOffset, EncKey.Length);
                        Array.ConstrainedCopy(EncSLIC, 0, patch, SLICOffset, EncSLIC.Length);
                        patch[patch.Length - 2] = patchOEMID;
                        patch[patch.Length - 3] = BIOSRW;
                        patch[patch.Length - 4] = EBDAIgnored;
                        Array.ConstrainedCopy(patchJMP, 0, file, 1, patchJMP.Length);
                        Array.ConstrainedCopy(patch, 0, file, patchOffset, patch.Length);
                        Utils.WriteFile(file, vi.VolumePath + "bootmgr");
                        Utils.SetFileTimes(vi.VolumePath + "bootmgr", Created, LastAccess, LastWrite);
                        File.SetAttributes(vi.VolumePath + "bootmgr", FileAttributes.System |
                            FileAttributes.ReadOnly |
                            FileAttributes.Archive |
                            FileAttributes.Hidden);
                        Utils.GiveOwnFile(vi.VolumePath + "bootmgr");
                    }
                    else
                    {
                        throw new System.IO.FileLoadException("bootmger length incorrect");
                    }
                }
            }
        }

        public static void UnpatchBootmgr(ref List<Utils.VolumeInfo> VolInfo)
        {
            foreach (Utils.VolumeInfo vi in VolInfo)
            {
                if (vi.VolumeName == "System Reserved" || vi.MountPoints.Contains("C:\\"))
                {
                    DateTime LastWrite = File.GetLastWriteTimeUtc(vi.VolumePath + "bootmgr");
                    DateTime LastAccess = File.GetLastAccessTimeUtc(vi.VolumePath + "bootmgr");
                    DateTime Created = File.GetCreationTimeUtc(vi.VolumePath + "bootmgr");
                    Utils.TakeOwnFile(vi.VolumePath + "bootmgr");
                    File.SetAttributes(vi.VolumePath + "bootmgr", FileAttributes.Normal);
                    byte[] file = Utils.ReadFile(vi.VolumePath + "bootmgr");
                    if (file != null && file.Length == 383786)
                    {
                        Array.ConstrainedCopy(unpatchJMP, 0, file, 1, patchJMP.Length);
                        Array.Clear(file, 0x3EE0, 0x13F8);
                        Utils.WriteFile(file, vi.VolumePath + "bootmgr");
                        Utils.SetFileTimes(vi.VolumePath + "bootmgr", Created, LastAccess, LastWrite);
                        Utils.GiveOwnFile(vi.VolumePath + "bootmgr");
                        File.SetAttributes(vi.VolumePath + "bootmgr", FileAttributes.System |
                            FileAttributes.ReadOnly |
                            FileAttributes.Archive |
                            FileAttributes.Hidden);
                    }
                    else
                    {
                        throw new System.IO.FileLoadException("bootmger length incorrect");
                    }
                }
            }
        }
    } // end class
} // end namespace
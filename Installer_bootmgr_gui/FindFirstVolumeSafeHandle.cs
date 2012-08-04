using System;
using System.Collections.Generic;
using System.Text;
using Microsoft.Win32.SafeHandles;

namespace WindSLIC_bootmgr
{
    public class FindFirstVolumeSafeHandle : SafeHandleZeroOrMinusOneIsInvalid
    {
        private FindFirstVolumeSafeHandle()
            : base(true)
        {
        }

        public FindFirstVolumeSafeHandle(IntPtr preexistingHandle, bool ownsHandle)
            : base(ownsHandle)
        {
            SetHandle(preexistingHandle);
        }

        protected override bool ReleaseHandle()
        {
            return Imports.FindVolumeClose(handle);
        }
    }
}

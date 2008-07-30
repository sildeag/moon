//
// Surface.cs
//
// Authors:
//   Rolf Bjarne Kvinge (rkvinge@novell.com)
//
// Copyright 2008 Novell, Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//


using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using Mono;

namespace Mono
{	
	/*
	 *  The managed equivalent of the unmanaged Surface
	 */
	public class Surface
	{
		private IntPtr native;
		private static Dictionary<Type,ManagedType> types = new Dictionary<Type,ManagedType> ();
		private static object sync_object = new object ();

		public IntPtr Native {
			get { return native; }
		}
		
		public Surface (IntPtr native)
		{
			this.native = native;
		}
	
		~Surface()
		{
			foreach (ManagedType ti in types.Values) {
				ti.gc_handle.Free ();
			}
		}
		
		public ManagedType FindType (Type type)
		{
			ManagedType info;
			ManagedType parent;
			
			if (types.TryGetValue (type, out info))
				return info;
			
			if (type.BaseType == null || type.BaseType == typeof (object)) {
				parent = null;
			} else {
				parent = FindType (type.BaseType);
			}
			
			return RegisterManagedType (type, parent);
		}
		

		private ManagedType RegisterManagedType (Type type, ManagedType parent)
		{
			ManagedType info;
		
			Console.WriteLine ("Surface.RegisterManagedType ({0}, {1})", type == null ? "null" : type.FullName, parent);
			
			lock (sync_object) {
				info = new ManagedType ();
				info.type = type;
				info.gc_handle = GCHandle.Alloc (type);
				info.parent = parent;
				info.native_handle = NativeMethods.surface_register_managed_type (native, type.FullName, GCHandle.ToIntPtr (info.gc_handle), parent != null ? parent.native_handle : 0);
				
				types.Add (type, info);
			}
			
			return info;
		}
				
	}
}

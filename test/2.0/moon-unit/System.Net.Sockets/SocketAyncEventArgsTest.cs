//
// Unit tests for System.Net.Sockets.SocketAsyncEventArgs
//
// Contact:
//   Moonlight List (moonlight-list@lists.ximian.com)
//
// Copyright (C) 2009-2010 Novell, Inc (http://www.novell.com)
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
using System.Net;
using System.Net.Sockets;

using Mono.Moonlight.UnitTesting;
using Microsoft.Silverlight.Testing;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace MoonTest.System.Net.Sockets {

	[TestClass]
	public class SocketAsyncEventArgsTest {

		[TestMethod]
		public void Defaults ()
		{
			SocketAsyncEventArgs e = new SocketAsyncEventArgs ();
			Assert.IsNull (e.Buffer, "Buffer");
			Assert.IsNull (e.BufferList, "BufferList");
			Assert.AreEqual (0, e.BytesTransferred, "BytesTransferred");
			Assert.IsNull (e.ConnectByNameError, "ConnectByNameError");
			Assert.IsNull (e.ConnectSocket, "ConnectSocket");
			Assert.AreEqual (0, e.Count, "Count");
			Assert.AreEqual (SocketAsyncOperation.None, e.LastOperation, "LastOperation");
			Assert.AreEqual (0, e.Offset, "Offset");
			Assert.IsNull (e.RemoteEndPoint, "RemoteEndPoint");
			Assert.AreEqual (SocketClientAccessPolicyProtocol.Tcp, e.SocketClientAccessPolicyProtocol, "SocketClientAccessPolicyProtocol");
			Assert.AreEqual (SocketError.Success, e.SocketError, "SocketError");
			Assert.IsNull (e.UserToken, "UserToken");
		}

		[TestMethod]
		public void ClientAccessPolicyProtocol ()
		{
			SocketAsyncEventArgs e = new SocketAsyncEventArgs ();
			Assert.AreEqual (SocketClientAccessPolicyProtocol.Tcp, e.SocketClientAccessPolicyProtocol, "default-tcp");
			e.SocketClientAccessPolicyProtocol = SocketClientAccessPolicyProtocol.Http;
			Assert.AreEqual (SocketClientAccessPolicyProtocol.Http, e.SocketClientAccessPolicyProtocol, "http");
			Assert.Throws<ArgumentException> (delegate {
				e.SocketClientAccessPolicyProtocol = (SocketClientAccessPolicyProtocol) Int32.MinValue;
			}, "invalid");
		}
	}
}


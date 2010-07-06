//
// System.Windows.Browser.HtmlPage class
//
// Contact:
//   Moonlight List (moonlight-list@lists.ximian.com)
//
// Copyright (C) 2007, 2009-2010 Novell, Inc (http://www.novell.com)
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

using System.Collections.Generic;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Windows.Interop;

using Mono;
using Mono.Xaml;

namespace System.Windows.Browser {

	public static class HtmlPage {

		private static BrowserInformation browser_info;
		private static HtmlWindow window;
		private static HtmlDocument document;
		private static HtmlElement plugin;
		private static Dictionary<string, Type> scriptableTypes;
		private static int last_user_initiated_event;

		static HtmlPage ()
		{
			scriptableTypes = new Dictionary<string, Type> ();

			if (PluginHost.Handle != IntPtr.Zero) {
				// we don't call RegisterScriptableObject since we're registering a private type
				ScriptObject services = new ManagedObject (HostServices.Services);
				NativeMethods.moonlight_scriptable_object_register (PluginHost.Handle, "services", services.Handle);
			}
		}

		static internal Dictionary<string, Type> ScriptableTypes {
			get { return scriptableTypes;}
		}


		public static BrowserInformation BrowserInformation {
			get {
				CheckHtmlAccess();

				if (browser_info == null)
					browser_info = new BrowserInformation (Window);

				return browser_info;
			}
		}
		
		public static bool IsEnabled {		
			get {
				return !Application.Current.IsRunningOutOfBrowser && Application.Current.Host.Settings.EnableHTMLAccess;
			}
		}
		
		public static HtmlDocument Document {
			get {
				CheckHtmlAccess();

				if (document == null)
					document = HtmlObject.GetPropertyInternal<HtmlDocument> (IntPtr.Zero, "document");

				return document;
			}
		}

		static void CheckName (string name, string parameterName)
		{
			if (name == null)
				throw new ArgumentNullException (parameterName);
			if ((name.Length == 0) || (name.IndexOf ('\0') != -1))
				throw new ArgumentException (parameterName);
		}

		public static void RegisterScriptableObject (string scriptKey, object instance)
		{
			// no call to CheckHtmlAccess(); -- see DRT364
			CheckName (scriptKey, "scriptKey");
			if (instance == null)
				throw new ArgumentNullException ("instance");
			Type t = instance.GetType ();
			if (!t.IsPublic && !t.IsNestedPublic)
				throw new InvalidOperationException ("'instance' type is not public.");

			if (!ManagedObject.IsScriptable (t))
				throw new ArgumentException ("No public [ScriptableMember] method was found.", "instance");

			ScriptObject sobj = instance as ScriptObject;
			if (sobj == null)
				sobj = new ManagedObject (instance);

			NativeMethods.moonlight_scriptable_object_register (PluginHost.Handle, scriptKey, sobj.Handle);
		}

		public static void RegisterCreateableType (string scriptAlias, Type type)
		{
			// no call to CheckHtmlAccess(); -- see DRT365
			CheckName (scriptAlias, "scriptAlias");
			if (type == null)
				throw new ArgumentNullException ("type");

			if (!ManagedObject.IsCreateable (type))
				throw new ArgumentException (type.ToString (), "type");

			if (ScriptableTypes.ContainsKey (scriptAlias))
				throw new ArgumentException ("scriptAlias");

			ScriptableTypes [scriptAlias] = type;
		}

		public static void UnregisterCreateableType (string scriptAlias)
		{
			CheckName (scriptAlias, "scriptAlias");

			if (!ScriptableTypes.ContainsKey (scriptAlias))
				throw new ArgumentException ("scriptAlias");

			ScriptableTypes.Remove (scriptAlias);
		}

		public static HtmlWindow Window {
			get {
				CheckHtmlAccess();
				return UnsafeWindow;
			}
		}

		// some features, like PopupWindow, works (within some limits) without EnableHtmlAccess
		static HtmlWindow UnsafeWindow {
			get {
				if (window == null)
					window = ScriptObject.GetPropertyInternal<HtmlWindow> (IntPtr.Zero, "window");

				return window;
			}
		}

		public static HtmlElement Plugin {
			get {
				CheckHtmlAccess();

				if (plugin == null)
					plugin = new HtmlElement (NativeMethods.plugin_instance_get_browser_host (Mono.Xaml.XamlLoader.PluginInDomain));

				return plugin;
			}
		}

		public static bool IsPopupWindowAllowed {
			// There are three conditions to allow popups:
			get {
				// 1) AllowHtmlPopupWindow is true. True is the default value for same-domain applications 
				// but this needs to be explicit (in the html) for cross-domain applications
				if (!NativeMethods.plugin_instance_get_allow_html_popup_window (XamlLoader.PluginInDomain))
					return false;

				// 2) the action must be user-initiated event (e.g. a click)
				IntPtr p = Deployment.Current.Surface.Native;
				if (!NativeMethods.surface_is_user_initiated_event (p))
					return false;

				// 3) a user-initiated event can only be used once (but this property can be called many times
				// by user code or by PopupUp itself). No flood!
				int user_initiated_event = NativeMethods.surface_get_user_initiated_counter (p);
				return (last_user_initiated_event != user_initiated_event);
			}
		}

		[MonoTODO ("Moonlight does not turn off the popup-blocker before calling this")]
		public static HtmlWindow PopupWindow (Uri navigateToUri, string target, HtmlPopupWindowOptions options)
		{
			
			// TODO: documentation says this method turns off (temporarily) the browser popup blocker
			// http://msdn.microsoft.com/en-us/library/system.windows.browser.htmlpage.popupwindow(VS.95).aspx
			// and if JavaScript's window.open is restricted then we should return null (not the HtmlWindow)
			// On FF this looks controlled by privacy.popups.policy and privacy.popups.disable_from_plugins

			if (navigateToUri == null)
				throw new ArgumentNullException ("navigateToUri");

			if ((navigateToUri.Scheme != "http") && (navigateToUri.Scheme != "https"))
				throw new ArgumentException ("navigateToUri");

			if (!IsPopupWindowAllowed)
				return null;

			// used only once, reset event counter
			last_user_initiated_event = NativeMethods.surface_get_user_initiated_counter (Deployment.Current.Surface.Native);

			// if EnableHtmlAccess is not enabled then null/empty target are converted into "_blank"
			if (!IsEnabled && String.IsNullOrEmpty (target))
				target = "_blank";

			object popup = null;
			if (options == null) {
				popup = HtmlPage.UnsafeWindow.Invoke ("open", navigateToUri.ToString (), target);
			} else {
				popup = HtmlPage.UnsafeWindow.Invoke ("open", navigateToUri.ToString (), target, options.AsString ());
			}

			// LAMESPEC: confusing but it seems the popup works with (or without) IsEnabled but, unless IsEnabled
			// is true, the return value will be null (i.e. a popup you can't control).
			return IsEnabled ? (HtmlWindow) window : null;
		}

		// The HTML bridge can be disable by the plugin 'enableHTMLAccess' parameter (defaults to true)
		// or can be unavailable if the plugin is hosted outside a browser (e.g. inside an IDE)
		static void CheckHtmlAccess ()
		{
			if (!IsEnabled)
				throw new InvalidOperationException ("HTML Bridge is not enabled or available.");
		}

		// look for an iframe named '_sl_historyFrame' in the web page and throw if not found
		static internal void EnsureHistoryIframePresence ()
		{
			HtmlElement iframe = HtmlPage.Document.GetElementById ("_sl_historyFrame");
			if ((iframe == null) || (String.Compare (iframe.TagName, "iframe", StringComparison.OrdinalIgnoreCase) != 0))
				throw new InvalidOperationException ("missing <iframe id=\"_sl_historyFrame\">");
		}

		static internal string NavigationState {
			get {
				// if not enabled (e.g. OoB) then return Empty (i.e. not an exception)
				if (!IsEnabled)
					return String.Empty;

				EnsureHistoryIframePresence ();
				return HtmlPage.Window.CurrentBookmark;
			}
			set {
				CheckHtmlAccess ();
				EnsureHistoryIframePresence ();
				HtmlPage.Window.CurrentBookmark = value;
			}
		}
	}
}
